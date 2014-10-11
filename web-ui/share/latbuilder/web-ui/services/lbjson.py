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
from multiprocessing import Process
import sys, os, signal, platform

class LatBuilderError(Exception):
    pass

@ServiceMethod
def backend_version():
    try:
        return latbuilder.get_version()
    except OSError, e:
        raise LatBuilderError("Cannot execute the `latbuilder' program (" + e.strerror + ")")
    except:
        raise LatBuilderError("Unknown error")


def sentry_run(pid, fd):

    if platform.system() == 'Linux':
        try:
            fin = os.fdopen(fd)
            fin.read()
        finally:
            os.kill(pid, signal.SIGTERM)

    elif platform.system() == 'Windows':
        import ctypes
        ctypes.windll.user32.MessageBoxA(None,
                "Click OK to abort Lattice Builder",
                "Lattice Builder Web Interface",
                0)
        os.kill(pid, signal.SIGTERM)


@ServiceMethod
def latbuilder_exec(*args):

    process = latbuilder.LatBuilderProcess(*args) 
    cmd = ' '.join(arg.startswith('--') and arg or '"{}"'.format(arg)
            for arg in process.command)
    try:
        process.start()

        # we start a bogus process that waits for data on stdin
        # to detect if the connection is lost
        sentry = Process(target=sentry_run, args=(process.process.pid, sys.stdin.fileno()))
        sentry.start()

        r = process.result()
        sentry.terminate()
        if not r:
            if process.process.returncode == 0:
                raise LatBuilderError("Aborted")
            else:
                raise LatBuilderError("<strong>Command</strong>: " + cmd + "\n<strong>Ouput:</strong> " + process.output)
        else:
            return (cmd, r.lattice.size.points, r.lattice.gen, r.lattice.merit, r.seconds)
    except OSError, e:
        raise LatBuilderError("Cannot execute the `latbuilder' program (" + e.strerror + ")")


@ServiceMethod
def array_from_expr(expr, index, dimension):
    return [parsemath.eval(expr, {index: i + 1}) for i in range(dimension)]


if __name__ == "__main__":
    handleCGI()
