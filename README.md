# Lattice Builder

_A general software tool for constructing rank-1 lattice rules_

# Quick Start Guide

## Requirements

The following software must be installed in order to compile Lattice Builder:

* C++11-Compliant Compiler (e.g., [GCC](http://gcc.gnu.org) 4.7)
* [Boost C++ Libraries](http://www.boost.org/)
* [FFTW Library](http://www.fftw.org/)
* [Boost.Build](http://www.boost.org/boost-build2/) (nightly build recommended)


## Configuring Boost.Build

### Site-Specific Configuration

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


### Project-Specific Configuration

The appropriate compiler flags should set in the `project-config.jam` file,
which can be found at the root of the source distribution, in order to support
the C++11 standard.  For example, with GCC 4.7, one can set:

	project latsoft : requirements <cxxflags>"-std=c++11" ;


## Compiling Lattice Builder

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
