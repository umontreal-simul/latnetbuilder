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
from multiprocessing import cpu_count
from contextlib import contextmanager


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

    def __init__(self, name, url):
        self._name = name
        self._url = url
        os.makedirs(self.root, exist_ok=True)

    @property
    def root(self):
        return SourceCode.ROOT

    @property
    def name(self):
        return self._name

    @property
    def url(self):
        return self._url

    @property
    def archive(self):
        return self._getstate('archive')

    @property
    def source_dir(self):
        return self._getstate('source_dir')

    def _getstate(self, key, default=None):
        return SourceCode.STATE.get(self.name + '|src|' + key, default)

    def _setstate(self, key, value):
        SourceCode.STATE[self.name + '|src|' + key] = value

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
                        time.gmtime(saved_mtime + 1)))
            if stream:
                stream.close()
            logging.info('  {} already exists; checking if a newer version in available'.format(self.archive))
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
        if self.source_dir and os.listdir(self.source_dir):
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
            prefix = self.name
            target = os.path.join(target, self.name)

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

    # base directory that will contain everything
    ROOT = '.'
    # destination directory
    PREFIX = 'dist'
    # can be replaced with a persistent state
    STATE = dict()
    # number of CPU's to use for parallel build
    NUM_PROCESSES = 1

    def __init__(self, source):
        self._source = source
        os.makedirs(self.root, exist_ok=True)
        os.makedirs(self.build_dir, exist_ok=True)

    @property
    def name(self):
        return self._source.name

    @property
    def root(self):
        return BuildRules.ROOT

    @property
    def source_dir(self):
        return self._source.source_dir

    @property
    def build_dir(self):
        return os.path.join(self.root, self.name)

    @property
    def prefix(self):
        return BuildRules.PREFIX

    @property
    def configured(self):
        return self._check_configured()
    @property
    def built(self):
        return self._check_built()
    @property
    def installed(self):
        return self._check_installed()

    @property
    def configure_log(self):
        return os.path.join(self.root, '{}-configure.log'.format(self.name))
    @property
    def build_log(self):
        return os.path.join(self.root, '{}-build.log'.format(self.name))
    @property
    def install_log(self):
        return os.path.join(self.root, '{}-install.log'.format(self.name))

    # private methods
    def _getstate(self, key, default=None):
        return BuildRules.STATE.get(self.name + '|build|' + key, default)
    def _setstate(self, key, value):
        BuildRules.STATE[self.name + '|build|' + key] = value

    # interface methods
    def configure(self):
        self._source.unpack()
        logging.info('configuring {}'.format(self.name))
        with open(self.configure_log, 'w') as log:
            self._do_configure(log)
        if not self.configured:
            raise BuildRules.Error('  failed configuring {}'.format(self.name))
        logging.info('  successfully configured {}'.format(self.name))
    def build(self):
        if not self.configured:
            self.configure()
        logging.info('building {}'.format(self.name))
        with open(self.build_log, 'w') as log:
            self._do_build(log)
        if not self.configured:
            raise BuildRules.Error('  failed building {}'.format(self.name))
        logging.info('  successfully built {}'.format(self.name))
    def install(self):
        if not self.built:
            self.build()
        logging.info('installing {}'.format(self.name))
        with open(self.install_log, 'w') as log:
            self._do_install(log)
        if not self.installed:
            raise BuildRules.Error('  failed installing {}'.format(self.name))
        logging.info('  successfully installed {} into {}'.format(self.name, self.prefix))

    # abstract methods
    def _do_configure(self):
        pass
    def _do_build(self):
        pass
    def _do_install(self):
        pass
    def _check_configured(self):
        return False
        #return self._getstate('configured', False)
    def _check_built(self):
        return False
        #return self._getstate('built', False)
    def _check_installed(self):
        return False
        #return self._getstate('installed', False)

class BoostBuildRules(BuildRules):
    def __init__(self, source):
        super().__init__(source)
    @property
    def siteconfig(self):
        return os.path.join(self.prefix, 'share', 'boost-build', 'site-config.jam')
    def _do_configure(self, log):
        shutil.rmtree(self.build_dir)
        shutil.copytree(self.source_dir, self.build_dir, symlinks=True,
                ignore=shutil.ignore_patterns('*.log'))
        with working_directory(self.build_dir):
            subprocess.check_call(
                    [os.path.join('.', 'bootstrap.sh')],
                    stdout=log,
                    stderr=subprocess.STDOUT)
    def _do_build(self, log):
        pass
    def _do_install(self, log):
        with working_directory(self.build_dir):
            subprocess.check_call([
                os.path.join('.', 'b2'),
                'install',
                '--prefix={}'.format(self.prefix)],
                stdout=log,
                stderr=subprocess.STDOUT)
        if not os.path.exists(self.siteconfig):
            f = open(self.siteconfig, 'w')
            f.write("project site-config ;\nusing gcc ;\n")
            f.close()

    def _check_configured(self):
        return os.path.exists(os.path.join(self.build_dir, 'bootstrap.log'))
    def _check_built(self):
        return self.configured
    def _check_installed(self):
        return os.path.exists(self.siteconfig)

class BoostRules(BuildRules):
    def __init__(self, source):
        super().__init__(source)
    def configure(self):
        super().configure()
    def build(self):
        super().build()
    def install(self):
        super().install()

class FftwRules(BuildRules):
    def __init__(self, source):
        super().__init__(source)
    def configure(self):
        super().configure()
    def build(self):
        super().build()
    def install(self):
        super().install()

class GccRules(BuildRules):
    def __init__(self, source):
        super().__init__(source)
    def configure(self):
        super().configure()
    def build(self):
        super().build()
    def install(self):
        super().install()

class Mingw64Rules(BuildRules):
    def __init__(self, source):
        super().__init__(source)
    def configure(self):
        super().configure()
    def build(self):
        super().build()
    def install(self):
        super().install()

class Mingw32Rules(BuildRules):
    def __init__(self, source):
        super().__init__(source)
    def configure(self):
        super().configure()
    def build(self):
        super().build()
    def install(self):
        super().install()

##############################################################################
# Global Functions
##############################################################################

def parse_command_line():
    parser = argparse.ArgumentParser(
            description='Setup a build environment.')
    parser.add_argument(
            "envtype",
            choices=['native', 'mingw64', 'mingw32'],
            help="environment type")
    parser.add_argument(
            "prefix",
            help="directory in which to install the build environment")
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
    parser.add_argument(
            "-v", "--verbose",
            help="increase output verbosity",
            action='store_true')
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
    statefile = os.path.join(args.root, '{}-state.db'.format(args.envtype))
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
    SourceCode.ROOT = os.path.join(args.root, 'src')
    BuildRules.ROOT = os.path.join(args.root, '{}-build'.format(args.envtype))
    SourceCode.STATE = persistent_state
    BuildRules.STATE = persistent_state
    BuildRules.NUM_PROCESSES = args.processes
    BuildRules.PREFIX = args.prefix

    bb_src = SourceCode(
            'Boost.Build',
            'http://www.boost.org/boost-build2/boost-build.tar.bz2')

    boost_src = SourceCode(
            'Boost',
            'http://sourceforge.net/projects/boost/files/boost/1.53.0/boost_1_53_0.tar.bz2/download')

    fftw_src = SourceCode(
            'FFTW',
            'http://www.fftw.org/fftw-3.3.3.tar.gz')

    bb_rules = BoostBuildRules(bb_src)
    if not bb_rules.installed: bb_rules.install()
