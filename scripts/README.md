# Building and Packaging Scripts

These scripts in this directory create distributable packages for Lattice
Builder for all supported platforms (32-bit and 64-bit Linux and Windows)
through cross-compilation.  They are meant to be run under a Linux environment
(contributions to support other platforms are welcome).


## Requirements

* [Python 3.3](http://python.org/) to execute the build script.
* A compiler or a cross-compiler, depending on the target platform:
    * [GCC 4.7 or later](http://gcc.gnu.org/) for 32- and 64-bit Linux;
    * [MinGW](http://www.mingw.org/) for 32-bit Windows;
    * [MinGW-w64](http://mingw-w64.sourceforge.net/) for 64-bit Windows.
* [pyjs](http://pyjs.org/) to build the web interface to Lattice Builder.


## Usage

Usage information can be obtained with:

	./build.py --help


## Usage Example

To build the 64-bit Linux package:

	./build.py -t x86_64 -r /tmp/buildtmp -p /tmp/buildenv b2 boost fftw latbuilder

This will store the temporary files (sources and object files) under the
`/tmp/buildtmp` directory, install the build environment (Lattice Builder
requirements: the Boost.Build build system and the Boost and FFTW libraries)
under the `/tmp/buildenv` directory, then create a Lattice Builder package in
the current working directory.

To build every target:

	for target in x86_64 i686 mingw32 mingw64
	do
		./build.py -t $target \
			-r /tmp/latbuilder \
			-p /tmp/latbuilder/$target-dist \
			b2 boost fftw latbuilder
	done
