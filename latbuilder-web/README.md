# Lattice Builder Web Interface


## Requirements

- [Python 2.7](http://python.org/download/)


## Usage

This application must be built with [pyjs](http://pyjs.org/) with the following
command line:

	pyjsbuild --no-compile-inplace \
                --output=share/latbuilder-web LatBuilderWeb

after creating the `share` subdirectory.

The `latbuilder` executable program, obtained by compiling Lattice Builder, must
be copied to the `bin` subdirectory.

Then the application can be used by running:

	bin/LatBuilderWeb.py

and by connecting a browser to the [application port](http://localhost:8000/).
