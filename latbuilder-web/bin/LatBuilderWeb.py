#!/usr/bin/env python2

import os
import subprocess

if __name__ == '__main__':
    here = os.path.dirname(os.path.abspath(__file__))
    serve = os.path.join(os.path.dirname(here), 'share', 'latbuilder-web', 'serve.py')
    path = here + os.pathsep + os.environ.get('PATH', os.defpath)
    os.environ['PATH'] = path
    subprocess.call(serve, shell=True)
