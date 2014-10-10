# Packaging Automation

This waf script creates distributable packages for Lattice
Builder for all supported platforms (32-bit and 64-bit Linux and Windows)
through cross-compilation.  They are meant to be run under a Linux environment
(contributions to support other platforms are welcome).


## Requirements

* A compiler or a cross-compiler, depending on the target platform:
    * [GCC 4.8 or later](http://gcc.gnu.org/) for 32- and 64-bit Linux;
    * [MinGW](http://www.mingw.org/) for 32-bit Windows;
    * [MinGW-w64](http://mingw-w64.sourceforge.net/) for 64-bit Windows.


## Usage

Configure the output directory, say `outdir`, with:

    ../waf -o outdir

Then build the packages:

    ../waf linux32
    ../waf linux64
    ../waf win32
    ../waf win64

The respective packages can be found in the `linux32`, `linux64`, `win32`,
`win64` subdirectories under the `outdir` directory.
