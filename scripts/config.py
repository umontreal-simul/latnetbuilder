# Copyright (c) 2013 David Munger, Pierre L'Ecuyer, Université de Montréal.
# 
# This file is part of Lattice Builder.
# 
# Lattice Builder is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# Lattice Builder is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with Lattice Builder.  If not, see <http://www.gnu.org/licenses/>.

##############################################################################
# General Settings
##############################################################################

import os

LATSOFT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

MINGW32 = 'i486-mingw32'
MINGW64 = 'x86_64-w64-mingw32'

SOURCES = {
'b2':           'http://www.boost.org/boost-build2/boost-build.tar.bz2',
'boost':        'http://sourceforge.net/projects/boost/files/boost/1.55.0/boost_1_55_0.tar.bz2/download',
'fftw':         'http://www.fftw.org/fftw-3.3.3.tar.gz',
'latbuilder':   'git:file://' + LATSOFT,
}
