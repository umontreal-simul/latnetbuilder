# Lattice Builder Quick Start Guide

_A general software tool for constructing rank-1 lattice rules_

You can either download a (pre-compiled) [binary  distribution of Lattice
Builder](#downloading-binaries) and start using it
right away, or [compile it from source](#compiling-from-source).


## Downloading Binaries

The easiest way to start using Lattice Builder is to [download a binary
distribution](https://github.com/mungerd/latbuilder/downloads) of Lattice
Builder, that includes the executable `latbuilder` program, together with
the Lattice Builder library and documentation.

The binary distribution packages have the following directory structure:

* **bin**: contains the executable `labuilder` program;
* **include**: contains the C++ header files necessary to use the Lattice
  Builder library;
* **lib**: contains the binary Lattice Builder library;
* **share/doc/latbuilder/html**: contains the HTML documentation;
* **share/doc/latbuilder/examples**: contains examples on using the Lattice
  Builder library;

The binary packages do not include an installation program.  To use the Lattice
Builder program, simply unpack the binary distribution package and move to its
root directory, then, e.g., to construct, component-by-component, a rank-1
lattice rule with 8191 points in 5 dimensions using the P-alpha criterion with
alpha=2 and with uniform product weights with value 0.1, issue the following
command on Linux:

	bin/latbuilder -l ordinary -n 8191 -d 5 -m CS:sum:P2 -w product:0.1 -c CBC

or, on Window systems:

	bin\latbuilder -l ordinary -n 8191 -d 5 -m CS:sum:P2 -w product:0.1 -c CBC

Try `bin/latbuilder --help` or read **share/doc/latbuilder/html/cmdtut.html**
for more information on using the Lattice Builder program.


## Compiling From Source

### Requirements

The following software must be installed in order to compile Lattice Builder:

* [Git](http://git-scm.com/)
* [C++11](http://www.iso.org/iso/iso_catalogue/catalogue_tc/catalogue_detail.htm?csnumber=50372)-Compliant Compiler (e.g., [GCC](http://gcc.gnu.org) 4.7)
* [Boost C++ Libraries](http://www.boost.org/)
* [FFTW Library](http://www.fftw.org/)
* [Boost.Build](http://www.boost.org/boost-build2/) (nightly build recommended)


### Configuring Boost.Build

Boost.Build needs to be told where to find the Boost and FFTW headers and how to
link to these libraries, via the `site-config.jam` configuration that can be
found under the directory `<b2-prefix>/share/boost-build`, where `<b2-prefix>`
is the prefix under which Boost.Build is installed.  If all libraries are
installed under standard paths, you can put the following contents into the
`site-config.jam` file:

	project site-config ;

	using gcc ;

	alias boost ; # header-only

	lib boost_program_options boost_system fftw3 ;
	lib boost_chrono : boost_system rt : <name>boost_chrono ;
	lib rt : : <link>shared ; # forced shared linkage

Otherwise, customize the following contents to suit your system configuration,
by replacing the `/opt/fftw3` and `/opt/boost` prefixes with the proper paths:

	project site-config ;
	
	lib fftw3 : : : <search>/opt/fftw3/lib/ : <include>/opt/fftw3/include/ ;

	alias boost : : : : <include>/opt/boost/include/ ;
	
	lib boost_program_options boost_system : :
		: <search>/opt/boost/lib/ : <include>/opt/boost/include/ ;

	lib boost_chrono : boost_system rt : <name>boost_chrono
		: <search>/opt/boost/lib/ : <include>/opt/boost/include/ ;

	lib rt : : <link>shared ;

The compilation process generates intermediate files; Boost.Build can be told to
put them in the temporary directory `/tmp/b2` b changing the project line of the
`site-config.jam` file to:

	project site-config : build-dir /tmp/b2 ;

**Remark:** If you do not have sufficient permissions to modify
the ̀`site-config.jam` file, you can resort to creating a `user-config.jam` file
at the root of your home directory and by modifying this file instead of the
`site-config.jam` file.


### Downloading from GitHub

Clone the Lattice Builder repository with:

	git clone git://github.com/mungerd/latbuilder.git

Then, move to the root directory of the repository with:

	cd latbuilder


### Configuring the Compiler to Use the C++11 Standard

The appropriate compiler flags should set in the `project-config.jam` file,
which can be found at the root of the source distribution, in order to support
the C++11 standard.  For example, with GCC 4.7, one can set:

	project latsoft : requirements <cxxflags>"-std=c++11" ;


### Compiling Lattice Builder

Change the current directory to the root or the source distribution.
First, compile the auxiliary tools that are needed to generate part of the
source code in Lattice Builder, with:

	b2 /tools

Then, prepare a binary distribution of Lattice Builder with:

	b2 /latbuilder//install --prefix=<dist-prefix>

where `<dist-prefix>` must be replaced with the directory into which Lattice
Builder should be installed.  The above command installs the `latbuilder`
executable under the `<dist-prefix>/bin` directory.


## Using the Lattice Builder Program

If the compilation suceeded, the following command should output the installed 
version of Lattice Builder:

	<dist-prefix>/bin/latbuilder --version

Help on usage can also be obtained by using the `--help` switch.


## Using the Lattice Builder Library

An example Jamfile and an example Makefile that can be modified to build a
project that uses the Lattice Builder application programming interface (API)
can be found under the `<dist-prefix>/share/doc/examples/myproject` directory,
together with a very simple piece of code.

For more information and a tutorial, refer to the HTML documentation in the
`<dist-prefix>/share/doc/html` directory.


## Frequently Asked Questions

### What construction type should I use to find good high-dimensional lattices in reasonable time?

Fast CBC with a coordinate-symmetric figure of merit.

For example, the following command line performs a fast CBC construction using
the weighted P-alpha criterion with alpha=2 and with product weights giving
equal the same weight of 0.01 to every coordinate (this means a weight of 1e-2
for projections of order 1, of 1e-4 for projections of order 2, of 1e-6 for
projections of order 3, etc.):

	latbuilder -l ordinary -n 2^16 -d 100 -m CS:sum:P2 -c fast-CBC -w product:0.01

The above search is for n=2^16=65,536 points in dimension 100.  Lattice Builder
does that very quickly.

