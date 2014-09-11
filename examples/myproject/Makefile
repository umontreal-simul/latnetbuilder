# Copyright (c) 2012 David Munger, Pierre L'Ecuyer, Université de Montréal.
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

# Example Makefile

# installation prefix of Lattice Builder
LATBUILDER = /usr

INCLUDES = -I$(LATBUILDER)/include
LIBS = -L$(LATBUILDER)/lib -llatbuilder -llatcommon -lboost_program_options -lboost_chrono -lboost_system -lfftw3

# production
CXXFLAGS = -Wall -O2 -DNDEBUG

# debugging
#CXXFLAGS = -Wall -O0 -g

LDFLAGS = -Wl,-R,"$(LATBUILDER)/lib"

%.o: %.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(INCLUDES) -std=c++11 -c $< -o $@

myproject: myproject.o
	$(CXX) $(LDFLAGS) $< $(LIBS) -o $@
