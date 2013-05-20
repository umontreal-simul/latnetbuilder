#!/usr/bin/env python2

import sys, os
import subprocess

if __name__ == '__main__':
    here = os.path.dirname(os.path.abspath(__file__))
    serve = os.path.join(os.path.dirname(here), 'share', 'latbuilder-web', 'serve.py')
    sys.path.insert(0, here)
    subprocess.call(serve, shell=True)
