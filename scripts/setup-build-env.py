#!/usr/bin/env python3

import logging
import subprocess
import argparse
import urllib.request, urllib.error, urllib.parse
import time
import sys, os, shutil
import math
import shelve
import mimetypes
import tarfile, zipfile
import platform, tempfile
import textwrap
import re
from multiprocessing import cpu_count
from contextlib import contextmanager


##############################################################################
# Source URL's
##############################################################################

SOURCES = {
        'b2':       'http://www.boost.org/boost-build2/boost-build.tar.bz2',
        'boost':    'http://sourceforge.net/projects/boost/files/boost/1.53.0/boost_1_53_0.tar.bz2/download',
        'fftw':     'http://www.fftw.org/fftw-3.3.3.tar.gz',
        }

##############################################################################
# Run a command, check output, and report
##############################################################################

def run_command(command, log):
    try:
        out = subprocess.check_output(command, stderr=subprocess.STDOUT)
        log.write(out.decode())
    except subprocess.CalledProcessError as err:
        out = err.output.decode()
        log.write(out)
        logging.error('  command failed with status {}: {}'.format(
            err.returncode,
            ' '.join(command)))
        logging.error('  command output:')
        for line in out.split('\n'):
            logging.error('  | ' + line)
        raise

##############################################################################
# Working Directory Context Manager
##############################################################################

@contextmanager
def working_directory(path):
    old = os.getcwd()
    os.chdir(path)
    yield
    os.chdir(old)

##############################################################################
# Environment Context Manager
##############################################################################

@contextmanager
def environment(**kwargs):
    old = dict(os.environ)
    os.environ.update(kwargs)
    yield
    os.environ.clear()
    os.environ.update(old)

##############################################################################
# Source Code Packages
##############################################################################

class SourceCode:
    # base directory that will contain everything
    ROOT = '.'
    # can be replaced with a persistent state
    STATE = dict()
    # time format used by HTTP servers
    TIMEFMT = '%a, %d %b %Y %H:%M:%S %Z'

    @staticmethod
    def as_date(date):
        if date is None:
            return None
        try:
            return time.strptime(date, SourceCode.TIMEFMT)
        except ValueError:
            return None

    @staticmethod
    def size_str(size):
        """Returns a string representing the size with human readable units."""
        units = ['B', 'kB', 'MB', 'GB', 'TB']
        rank = int(math.log(size, 1024))
        return '{:.1f} {}'.format(size / (1024**rank), units[rank])

    def __init__(self, package, url=None):
        self._package = package
        self._url = url or SOURCES[package]
        os.makedirs(self.root, exist_ok=True)

    @property
    def root(self):
        return SourceCode.ROOT

    @property
    def package(self):
        return self._package

    @property
    def url(self):
        return self._url

    @property
    def archive(self):
        return self._getstate('archive')

    @property
    def source_dir(self):
        return self._getstate('source_dir')

    @property
    def unpacked(self):
        return self.source_dir and os.path.exists(self.source_dir) and os.listdir(self.source_dir)

    def _getstate(self, key, default=None):
        return SourceCode.STATE.get(self.package + '|src|' + key, default)

    def _setstate(self, key, value):
        SourceCode.STATE[self.package + '|src|' + key] = value

    def download(self):
        logging.info('downloading {}'.format(self.url))

        request = urllib.request.Request(self.url)
        opener = urllib.request.build_opener()
        stream = None

        if not self.archive:
            # set archive name
            stream = opener.open(request)
            basename = os.path.basename(urllib.request.url2pathname(stream.geturl()))
            self._setstate('archive', os.path.join(self.root, basename))

        try:
            saved_mtime = os.path.getmtime(self.archive)
            request.add_header(
                    'If-Modified-Since',
                    time.strftime(
                        SourceCode.TIMEFMT,
                        time.gmtime(saved_mtime + 300)))
            if stream:
                stream.close()
            logging.info('  {} already exists'.format(self.archive))
            logging.info('  checking if a newer version in available')
        except FileNotFoundError:
            pass

        try:
            stream = opener.open(request)

            if stream.headers['Content-Type'].startswith('text/html'):
                raise RuntimeError('Cannot download URL: %s' % u.geturl())

            # retrieve headers
            mtime = SourceCode.as_date(stream.headers.get('Last-Modified', None))
            size = int(stream.headers.get('Content-Length', 0))

            # output information
            logging.info('  {:12s}: {}'.format('destination', self.archive))
            logging.info('  {:12s}: {}'.format('size', SourceCode.size_str(size)))

            if mtime:
                logging.info('  {:12s}: {}'.format('modified',
                    time.strftime(SourceCode.TIMEFMT, mtime)))

            # save URL
            f = open(self.archive, 'wb')
            f.write(stream.read())
            f.close()

            # adjust last-modified time
            if mtime:
                os.utime(self.archive, (time.time(), time.mktime(mtime)))

            logging.info('  successfully downloaded {}'.format(self.archive))

        except urllib.error.HTTPError as err:
            if err.code == 304:
                logging.info('  {} is recent enough'.format(self.archive))
                return
            raise


    def unpack(self):
        if not self.archive or not os.path.exists(self.archive):
            self.download()

        logging.info('unpacking {}'.format(self.archive))
        if self.unpacked:
            logging.info('  already unpacked; skipping')
            return

        target = self.root
             
        content, encoding = mimetypes.guess_type(self.archive)
        if content == 'application/x-tar':
            archive = tarfile.open(self.archive)
            prefix = os.path.commonprefix(archive.getnames())
        elif content == 'application/zip':
            archive = zipfile.open(self.archive)
            prefix = os.path.commonprefix(archive.namelist())
        else:
            raise RuntimeError('unknown archive type: {}'.format(content))

        # if no prefix, create one
        if len(prefix) == 0:
            prefix = self.package
            target = os.path.join(target, self.package)

        # extract files
        self._setstate('source_dir', os.path.join(target, prefix))
        os.makedirs(target, exist_ok=True)
        archive.extractall(target)

        logging.info('  successfully unpacked {}'.format(self.archive))

##############################################################################
# Build Rules
##############################################################################

class BuildRules:
    class Error(RuntimeError):
        def __init__(self, msg):
            super().__init__(msg)

    # target platform
    TARGET = platform.machine()
    # base directory that will contain everything
    ROOT = os.getcwd()
    # destination directory
    PREFIX = os.path.join(ROOT, 'dist')
    # can be replaced with a persistent state
    STATE = dict()
    # number of CPU's to use for parallel build
    NUM_PROCESSES = 1
    # temporary directory
    TMPDIR = tempfile.gettempdir()

    def __init__(self, package):
        self._source = SourceCode(package)
        os.makedirs(self.root, exist_ok=True)
        os.makedirs(self.build_dir, exist_ok=True)

    @property
    def package(self):
        return self._source.package

    @property
    def target(self):
        return BuildRules.TARGET

    @property
    def root(self):
        return BuildRules.ROOT

    @property
    def source_dir(self):
        return self._source.source_dir

    @property
    def build_dir(self):
        return os.path.join(self.root, self.package)

    @property
    def prefix(self):
        return BuildRules.PREFIX

    @property
    def tmpdir(self):
        return BuildRules.TMPDIR

    @property
    def num_processes(self):
        return BuildRules.NUM_PROCESSES

    @property
    def configured(self):
        return self._source.unpacked and self._check_configured()
    @property
    def built(self):
        return self.configured and self._check_built()
    @property
    def installed(self):
        return self.built and self._check_installed()

    @property
    def configure_log(self):
        return os.path.join(self.root, '{}-configure.log'.format(self.package))
    @property
    def build_log(self):
        return os.path.join(self.root, '{}-build.log'.format(self.package))
    @property
    def install_log(self):
        return os.path.join(self.root, '{}-install.log'.format(self.package))

    # private methods
    def _getstate(self, key, default=None):
        return BuildRules.STATE.get(self.package + '|build|' + key, default)
    def _setstate(self, key, value):
        BuildRules.STATE[self.package + '|build|' + key] = value

    # interface methods
    def clear(self):
        logging.info('clearing {}'.format(self.package))
        shutil.rmtree(self.build_dir)
    def configure(self):
        self._source.unpack()
        logging.info('configuring {}'.format(self.package))
        with open(self.configure_log, 'w') as log:
            self._do_configure(log)
        if not self.configured:
            raise BuildRules.Error('  failed configuring {}'.format(self.package))
        logging.info('  successfully configured {}'.format(self.package))
    def build(self):
        if not self.configured:
            self.configure()
        logging.info('building {}'.format(self.package))
        with open(self.build_log, 'w') as log:
            self._do_build(log)
        if not self.configured:
            raise BuildRules.Error('  failed building {}'.format(self.package))
        logging.info('  successfully built {}'.format(self.package))
    def install(self):
        if not self.built:
            self.build()
        logging.info('installing {}'.format(self.package))
        with open(self.install_log, 'w') as log:
            self._do_install(log)
        if not self.installed:
            raise BuildRules.Error('  failed installing {}'.format(self.package))
        logging.info('  successfully installed {} into {}'.format(self.package, self.prefix))

    # abstract methods
    def _do_configure(self):
        pass
    def _do_build(self):
        pass
    def _do_install(self):
        pass
    def _check_configured(self):
        return False
    def _check_built(self):
        return False
    def _check_installed(self):
        return False

class B2Rules(BuildRules):
    PROPERTIES = {
            'x86_64': {
                'requirements':     '',
                'compiler':         'gcc',
                'compiler-version': '',
                'compiler-command': '',
                'compiler-options': '',
                },
            'i686': {
                'requirements':     '<address-model>32',
                'compiler':         'gcc',
                'compiler-version': 'mingw32',
                'compiler-command': 'i486-mingw32-g++',
                'compiler-options': '',
                },
            'mingw32': {
                'requirements':     '<target-os>windows <address-model>32',
                'compiler':         'gcc',
                'compiler-version': 'mingw32',
                'compiler-command': 'i486-mingw32-g++',
                'compiler-options': '',
                },
            'mingw64': {
                'requirements':     '<target-os>windows',
                'compiler':         'gcc',
                'compiler-version': 'mingw64',
                'compiler-command': 'x86_64-w64-mingw32-g++',
                'compiler-options': '',
                },
            }
    def __init__(self):
        super().__init__('b2')
    @property
    def siteconfig(self):
        return os.path.join(self.prefix, 'share', 'boost-build', 'site-config.jam')
    def _do_configure(self, log):
        shutil.rmtree(self.build_dir)
        shutil.copytree(self.source_dir, self.build_dir, symlinks=True,
                ignore=shutil.ignore_patterns('*.log'))
        with working_directory(self.build_dir):
            run_command([os.path.join('.', 'bootstrap.sh')], log)
    def _do_build(self, log):
        pass
    def _do_install(self, log):
        with working_directory(self.build_dir):
            run_command([
                os.path.join('.', 'b2'),
                'install',
                '--prefix={}'.format(self.prefix),
                ], log)
        siteconfig = """
                import os ;
                import path ;

                local build-dir = [ path.join {tmpdir} [ os.environ USER ] b2 ] ;

                project site-config :
                    build-dir $(build-dir) :
                    requirements {requirements} ;

                using {compiler} :
                    {compiler-version} :
                    {compiler-command} :
                    {compiler-options} ;

                local prefix = {prefix} ;

                alias boost : : : :
                    <include>$(prefix)/include/ ;

                lib boost_program_options boost_system : : :
                    <search>$(prefix)/lib/ :
                    <include>$(prefix)/include/ ;

                lib boost_chrono : boost_system : <name>boost_chrono :
                    <search>$(prefix)/lib/ :
                    <include>$(prefix)/include/ ;

                lib fftw3 : : :
                    <search>$(prefix)/lib/ :
                    <include>$(prefix)/include/ ;
                """.format(
                        prefix=self.prefix,
                        tmpdir=self.tmpdir,
                        **B2Rules.PROPERTIES[self.target])
        with open(self.siteconfig, 'w') as f:
            f.write(textwrap.dedent(siteconfig))
            logging.info("  created {}".format(self.siteconfig))

    def _check_configured(self):
        return os.path.exists(self.configure_log) and \
                any(re.search('Bootstrapping is done', line)
                        for line in open(self.configure_log))
    def _check_built(self):
        return self.configured
    def _check_installed(self):
        return os.path.exists(self.siteconfig)

class BoostRules(BuildRules):
    def __init__(self):
        super().__init__('boost')
    @property
    def b2_path(self):
        return os.path.join(self.prefix, 'bin', 'b2')
    @property
    def b2_command(self):
        return [self.b2_path,
                '-j{}'.format(self.num_processes),
                '--stagedir={}'.format(self.build_dir),
                '--prefix={}'.format(self.prefix),
                '--layout=tagged'.format(self.prefix),
                'link=static,shared',
                'threading=single,multi',
                ]
    def _do_configure(self, log):
        with working_directory(self.source_dir):
            command = [
                os.path.join(self.source_dir, 'bootstrap.sh'),
                '--prefix={}'.format(self.prefix),
                '--with-bjam={}'.format(self.b2_path),
                '--with-toolset=gcc',
                '--without-icu',
                '--with-libraries=program_options,chrono,system',
                ]
            if os.path.exists('boost-build.jam'):
                os.remove('boost-build.jam')
            #logging.info('launching: {}'.format(' '.join(command)))
            run_command(command, log)
    def _do_build(self, log):
        with working_directory(self.source_dir):
            run_command(self.b2_command, log)
    def _do_install(self, log):
        with working_directory(self.source_dir):
            run_command(self.b2_command + ['install'], log)
    def _check_configured(self):
        return os.path.exists(self.configure_log) and \
                any(re.search('Bootstrapping is done', line)
                    for line in open(self.configure_log)) \
                and not os.path.exists(os.path.join(self.source_dir, 'boost-build.jam'))
    def _check_built(self):
        return os.path.exists(os.path.join(self.build_dir, 'lib', 'libboost_program_options.a'))
    def _check_installed(self):
        return os.path.exists(os.path.join(self.prefix, 'include', 'boost', 'config.hpp')) \
                and os.path.exists(os.path.join(self.prefix, 'lib', 'libboost_program_options.a'))

class FftwRules(BuildRules):
    CONFIGURE = {
            'x86_64': [],
            'i686': [],
            'mingw32': [
                '--host=i686-mingw32',
                '--disable-alloca',
                '--with-our-malloc16',
                '--with-windows-f77-mangling',
                '--disable-static',
                '--enable-sse2',
                ],
            'mingw64': [
                '--host=x86_64-w64-mingw32',
                '--disable-alloca',
                '--with-our-malloc16',
                '--with-windows-f77-mangling',
                '--disable-static',
                '--enable-sse2',
                ],
            }
    def __init__(self):
        super().__init__('fftw')
    def _do_configure(self, log):
        with working_directory(self.build_dir):
            command = [
                os.path.join(self.source_dir, 'configure'),
                '--prefix={}'.format(self.prefix),
                '--enable-shared',
                '--enable-threads',
                '--with-combined-threads',
                ] + FftwRules.CONFIGURE[self.target]
            run_command(command, log)
    def _do_build(self, log):
        with working_directory(self.build_dir):
            run_command(['make', '-j{}'.format(self.num_processes)], log)
    def _do_install(self, log):
        with working_directory(self.build_dir):
            run_command(['make', '-j{}'.format(self.num_processes), 'install'], log)
    def _check_configured(self):
        log = os.path.join(self.build_dir, 'config.log')
        return os.path.exists(log) and \
                any(re.search('configure: exit 0', line)
                    for line in open(log))
    def _check_built(self):
        return os.path.exists(os.path.join(self.build_dir, 'lib', 'libfftw3.a'))
    def _check_installed(self):
        return os.path.exists(os.path.join(self.prefix, 'include', 'fftw3.h')) \
                and os.path.exists(os.path.join(self.prefix, 'lib', 'libfftw3.a'))

##############################################################################
# Global Functions
##############################################################################

def parse_command_line():
    parser = argparse.ArgumentParser(
            description='Setup a build environment.')
    parser.add_argument(
            "-p", "--prefix",
            required=True,
            help="directory in which to install the build environment")
    parser.add_argument(
            "packages",
            choices=['gcc', 'fftw', 'b2', 'boost'],
            nargs='+',
            help="directory in which to install the build environment")
    parser.add_argument(
            "-t", "--target",
            choices=['i686', 'x86_64', 'mingw32', 'mingw64'],
            help="environment type",
            default=platform.machine())
    parser.add_argument(
            "-r", "--root",
            help="root working directory where to store sources and build files",
            default='.')
    parser.add_argument(
            "-j", "--processes",
            help="number of CPU's to use",
            default=cpu_count())
    #parser.add_argument(
    #        "-i", "--interactive",
    #        help="ask questions along the way",
    #        action='store_true')
    #parser.add_argument(
    #        "-v", "--verbose",
    #        help="increase output verbosity",
    #        action='store_true')
    return parser.parse_args()

def dump_state(state):
    for k, v in sorted(state.items()):
        print('  {:20s}: {}'.format(k, v))

##############################################################################
# Main
##############################################################################

if __name__ == '__main__':
    logging.basicConfig(
            level=logging.DEBUG,
            format='%(levelname)-8s %(message)s')
    args = parse_command_line()

    # persistent state
    statefile = os.path.join(args.root, '{}-state.db'.format(args.target))
    persistent_state = shelve.open(statefile)

    # clean up state at exit
    import atexit
    atexit.register(lambda: persistent_state.close())

    # show current state
    #if persistent_state:
    #    print('Last state:')
    #    dump_state(persistent_state)

    # ensure prefix is an absolute path
    if not os.path.isabs(args.prefix):
        args.prefix = os.path.join(os.getcwd(), args.prefix)

    # set static variables
    SourceCode.STATE = persistent_state
    BuildRules.STATE = persistent_state
    SourceCode.ROOT = os.path.join(args.root, 'src')
    BuildRules.ROOT = os.path.join(args.root, '{}-build'.format(args.target))
    BuildRules.NUM_PROCESSES = args.processes
    BuildRules.PREFIX = args.prefix
    BuildRules.TARGET = args.target

    def create_rules(target, package):
        RulesClass = eval(package.capitalize() + 'Rules')
        if package == 'gcc':
            raise NotImplementedError('gcc installation not implemented')
        return RulesClass()

    for package in args.packages:
        rules = create_rules(args.target, package)
        if not rules.installed:
            rules.install()
