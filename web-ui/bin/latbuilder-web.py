#!/usr/bin/env python

# Copyright (c) 2013 David Munger, Pierre L'Ecuyer, Universite de Montreal.
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


import os, sys
import subprocess

if __name__ == '__main__':
    here = os.path.dirname(os.path.abspath(__file__))
    serve = os.path.join(os.path.dirname(here), 'share', 'latbuilder', 'web-ui', 'serve.py')
    path = here + os.pathsep + os.environ.get('PATH', os.defpath)
    os.environ['PATH'] = path
    try:
        subprocess.call([sys.executable, serve])
    except KeyboardInterrupt:
        print("exiting")
