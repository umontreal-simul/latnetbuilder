#!/usr/bin/env python2

from jsonrpc import handleCGI, ServiceMethod
import latbuilder
import subprocess

@ServiceMethod
def execute(*args):
    try:
        r = latbuilder.execute(*args)
        return (r.lattice.size.points, r.lattice.gen, r.lattice.merit)
    except subprocess.CalledProcessError, e:
        return 'ERROR:\ncommand: ' + ' '.join(e.cmd) + '\nouput: ' + e.output

if __name__ == "__main__":
    handleCGI()
