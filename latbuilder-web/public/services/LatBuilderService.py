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
def backend_version():
    return latbuilder.get_version()


@ServiceMethod
def latbuilder_exec(*args):

    process = latbuilder.LatBuilderProcess(*args) 
    cmd = ' '.join(arg.startswith('--') and arg or '"{}"'.format(arg)
            for arg in process.command)
    process.start()

    try:
        r = process.result()
        return (cmd, r.lattice.size.points, r.lattice.gen, r.lattice.merit, r.seconds)
    except subprocess.CalledProcessError, e:
        return "ERROR:\ncommand: " + ' '.join(e.cmd) + "\nouput: " + e.output
    except OSError, e:
        return "ERROR:\ncannot execute the `latbuilder' program: " + e.strerror


@ServiceMethod
def array_from_expr(expr, index, dimension):
    return [parsemath.eval(expr, {index: i + 1}) for i in range(dimension)]


if __name__ == "__main__":
    handleCGI()
