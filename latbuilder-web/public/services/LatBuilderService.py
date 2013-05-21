#!/usr/bin/env python2

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
