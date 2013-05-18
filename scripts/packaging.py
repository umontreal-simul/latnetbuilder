import logging
import subprocess
import urllib.request, urllib.error, urllib.parse
import time
import os, shutil, re
import math
import mimetypes
import tarfile, zipfile
import platform, tempfile
from contextlib import contextmanager

__all__ = [
'run_command',
'find_files',
'working_directory',
'environment',
'SourceCode',
'BuildRules',
'PackageRules',
]

##############################################################################
# Run a command, check output, and report
##############################################################################

def run_command(command, log=None):
    try:
        if log:
            log.write('  running command: {}'.format(' '.join(command)))
        out = subprocess.check_output(command, stderr=subprocess.STDOUT)
        if log:
            log.write(out.decode())
        return out
    except subprocess.CalledProcessError as err:
        out = err.output.decode()
        if log:
            log.write(out)
        logging.error('  command output:')
        for line in out.split('\n'):
            logging.error('  | ' + line)
        logging.error('  command failed with status {}: {}'.format(
            err.returncode, ' '.join(command)))
        raise

##############################################################################
# Find Files
##############################################################################

def find_files(basedir, pattern):
    for root, dirnames, filenames in os.walk(basedir):
        for filename in filenames:
            if re.search(pattern, filename, re.IGNORECASE):
                yield os.path.join(root, filename)

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
    # check if newer sources are available online even when the archive exists
    CHECK_NEWER = False

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

    def __init__(self, package, url):
        self._package = package
        self._url = url
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
        if self.url.startswith('git:'):
            self._git_download(self.url[4:])
        else:
            self._http_download(self.url)

    def _git_download(self, url):
        logging.info('cloning git repository: {}'.format(url))
        self._setstate('source_dir',
                os.path.join(self.root, os.path.basename(url)))
        shutil.rmtree(self.source_dir)
        run_command(['git', 'clone', url, self.source_dir])

    def _http_download(self, url):
        logging.info('downloading {}'.format(url))

        request = urllib.request.Request(url)
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
            if SourceCode.CHECK_NEWER:
                logging.info('  checking if a newer version in available')
            else:
                return
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
        if self.url.startswith('git:'):
            return

        logging.info('unpacking {}'.format(self.archive))

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

    # target platform
    TARGET = platform.machine()

    # base directory that will contain everything
    ROOT = os.getcwd()

    # destination directory
    PREFIX = os.path.join(ROOT, 'dist')

    # number of CPU's to use for parallel build
    NUM_PROCESSES = 1

    # temporary directory
    TMPDIR = tempfile.gettempdir()

    class Error(RuntimeError):
        def __init__(self, msg):
            super().__init__(msg)

    def __init__(self, source):
        self._source = source
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

    # interface methods

    def clear(self):
        logging.info('clearing {}'.format(self.package))
        shutil.rmtree(self.build_dir)

    def configure(self):
        if not self._source.unpacked:
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
        if not self.built:
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


##############################################################################
# Package Rules
##############################################################################

class PackageRules(BuildRules):

    def __init__(self, source, archive):
        super().__init__(source)
        self.archive = archive

    def _archive_format(self):
        for f, exts, desc in shutil.get_unpack_formats():
            for ext in exts:
                if self.archive.endswith(ext):
                    return (self.archive[:-len(ext)], f)

    @property
    def packed(self):
        return self.installed and self._check_packed()

    @property
    def package_log(self):
        return os.path.join(self.root, '{}-package.log'.format(self.package))

    def pack(self):
        if not self.installed:
            self.install()
        logging.info('packaging {}'.format(self.package))
        with open(self.package_log, 'w') as log:
            self._do_pack(log)
        if not self.packed:
            raise BuildRules.Error('  failed packaging {}'.format(self.package))
        logging.info('  successfully packaged {} into {}'.format(
            self.package, self.archive))

    # abstract methods

    def _do_pack(self, log):
        name, fmt = self._archive_format()
        shutil.make_archive(
                name,
                fmt,
                os.path.dirname(self.build_dir),
                os.path.basename(self.build_dir))

    def _check_packed(self):
        return os.path.exists(self.archive)
