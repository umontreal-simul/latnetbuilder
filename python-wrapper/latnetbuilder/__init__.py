# -*- coding: utf-8 -*-
"""
Interface for the C++ program LatNetBuilder.

This module is an interface for the C++ program LatNetBuilder. 
It offers both an object-oriented interface through classes SearchLattice and SearchNet, 
and a Graphical-User Interface through the instance gui.
The GUI works in a Jupyter notebook, and can be displayed by displaying latnetbuilder.gui
"""

PATH_TO_LATNETBUILDER = 'latnetbuilder'
'''str: path to the latbuilder executable

If you used an installer (conda or waf with --build-conda), you don't have to modify the path.
Else, modify this path to match the current path to LatNetBuilder on your system.'''

import atexit
import os
import shutil
def _delete_archive():
    try:
        os.remove('latnetbuilder-results.tar.gz')
        os.remove('latnetbuilder-results.zip')
        shutil.rmtree('latnetbuilder_results', ignore_errors=True)
    except:
        pass
atexit.register(_delete_archive)

from .gui import gui
from .search import SearchLattice, SearchNet
from .generate_points import generate_points_digital_net, generate_points_ordinary_lattice
