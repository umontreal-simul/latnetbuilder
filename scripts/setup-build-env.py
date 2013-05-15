#!/usr/bin/env python3

import logging
import argparse
import urllib.request, urllib.error, urllib.parse
import time
import os, shutil
import math
import shelve
import mimetypes
import tarfile, zipfile
from multiprocessing import cpu_count

##############################################################################

##############################################################################


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
    # base directory that will contain everything
    ROOT = '.'
    # can be replaced with a persistent state
    STATE = dict()

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
    def build_dir(self):
        return os.path.join(self.root, self.name)

    @property
    def configured(self):
        return self._getstate('configured', False)

    @property
    def built(self):
        return self._getstate('built', False)

    @property
    def installed(self):
        return self._getstate('installed', False)

    def _getstate(self, key, default=None):
        return BuildRules.STATE.get(self.name + '|build|' + key, default)

    def _setstate(self, key, value):
        BuildRules.STATE[self.name + '|build|' + key] = value

    def configure(self):
        self._source.unpack()
        logging.info('configuring {}'.format(self.name))

    def build(self):
        logging.info('building {}'.format(self.name))

    def install(self):
        logging.info('installing {}'.format(self.name))

class BoostBuildRules(BuildRules):
    def __init__(self, source):
        super().__init__(source)
    def configure(self):
        super().configure()
    def build(self):
        super().build()
    def install(self):
        super().install()

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
            "-n", "--num-cpus",
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
    print('installing stuff to {}'.format(args.prefix))

    # persistent state
    statefile = os.path.join(args.root, '{}-state.db'.format(args.envtype))
    persistent_state = shelve.open(statefile)

    # clean up state at exit
    import atexit
    atexit.register(lambda: persistent_state.close())

    if persistent_state:
        print('Last state:')
        dump_state(persistent_state)

    # set static variables
    SourceCode.ROOT = os.path.join(args.root, 'src')
    BuildRules.ROOT = os.path.join(args.root, '{}-build'.format(args.envtype))
    SourceCode.STATE = persistent_state
    BuildRules.STATE = persistent_state

    bb_src = SourceCode(
            'Boost.Build.v2',
            'http://www.boost.org/boost-build2/boost-build.tar.bz2')

    boost_src = SourceCode(
            'Boost',
            'http://sourceforge.net/projects/boost/files/boost/1.53.0/boost_1_53_0.tar.bz2/download')

    fftw_src = SourceCode(
            'FFTW',
            'http://www.fftw.org/fftw-3.3.3.tar.gz')

    bb_rules = BoostBuildRules(bb_src)
    bb_rules.configure()
    bb_rules.build()
    bb_rules.install()
