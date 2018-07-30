"""
Interface for the C++ program LatNetBuilder.

This module is an interface for the C++ program LatNetBuilder. 
It offers both an object-oriented interface through classes SearchLattice and SearchNet, 
and a Graphical-User Interface through the instance gui.
The GUI works in a Jupyter notebook, and can be displayed by displaying latnetbuilder.gui
"""

PATH_TO_LATNETBUILDER = 'latnetbuilder'
'''str: path to the latbuilder executable

If you used an installer (conda or waf with --build-full-conda), you don't have to modify the path.
Else, modify this path to match the current path to LatNetBuilder on your system.'''

from .gui import gui
from .search import SearchLattice, SearchNet