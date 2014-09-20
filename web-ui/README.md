# Lattice Builder Web Interface


## Requirements

- [Python 2.7](http://python.org/download/)


## Usage

This application must be built with [pyjs](http://pyjs.org/) ([from 
GitHub](https://github.com/pyjs/pyjs)) with the following
command line:

    pyjsbuild --disable-compile-inplace \
              --output=share/latbuilder-web LatBuilderWeb

after creating the `share` subdirectory.

The `latbuilder` executable program, obtained by compiling Lattice Builder, must
be copied to the `bin` subdirectory.

Then the application can be used by running:

    bin/LatBuilderWeb.py

and by connecting a browser to the [application port](http://localhost:8080/).


### Patched version of pyjs

For the *Abort* function to work, a [patched version of 
pyjs](https://github.com/mungerd/pyjs/tree/abortpatch) is required.
It can be installed with:

    git clone --depth 1 --branch abortpatch https://github.com/mungerd/pyjs.git
    cd pyjs
    python setup.py install --home
