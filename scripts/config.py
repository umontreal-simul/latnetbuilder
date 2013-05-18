##############################################################################
# General Settings
##############################################################################

import os

LATSOFT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

MINGW32 = 'i486-mingw32'
MINGW64 = 'x86_64-w64-mingw32'

SOURCES = {
'b2':           'http://www.boost.org/boost-build2/boost-build.tar.bz2',
'boost':        'http://sourceforge.net/projects/boost/files/boost/1.53.0/boost_1_53_0.tar.bz2/download',
'fftw':         'http://www.fftw.org/fftw-3.3.3.tar.gz',
'latbuilder':   'git:file://' + LATSOFT,
}
