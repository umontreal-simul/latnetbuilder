#!/usr/bin/env python2

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


from jsonrpc import handleCGI, ServiceMethod
import parsemath
import latbuilder
import subprocess

@ServiceMethod
def execute(*args):
    try:
        r = latbuilder.execute(*args)
        return (r.lattice.size.points, r.lattice.gen, r.lattice.merit)
    except subprocess.CalledProcessError, e:
        return "ERROR:\ncommand: " + ' '.join(e.cmd) + "\nouput: " + e.output
    except OSError, e:
        return "ERROR:\ncannot execute the `latbuilder' program: " + e.strerror

@ServiceMethod
def make_product_weights(expr, dimension):
    return [parsemath.eval(expr, {'j': j + 1}) for j in range(dimension)]

@ServiceMethod
def make_order_weights(expr, dimension):
    return [parsemath.eval(expr, {'k': k + 1}) for k in range(dimension)]

if __name__ == "__main__":
    handleCGI()
