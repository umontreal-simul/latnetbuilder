# LatNet Builder
_A general software tool for constructing highly uniform point sets_

*Quasi-Monte Carlo methods* are often used as a replacement for [Monte
Carlo](http://en.wikipedia.org/wiki/Monte_Carlo_integration) to integrate
multidimensional functions.

Monte Carlo and quasi-Monte Carlo methods are used for the same purpose. The problem is to approximate the integral of a function over the unit hypercube as the average of the function evaluated at a set of points. Whereas Monte-Carlo uses a (pseudo)-random sequence of points, quasi-Monte Carlo uses a hihly uniform point set.
For certain problems, well-constructed point sets can dramatically reduce the integration error with respect to Monte Carlo.

*LatNet Builder* is a software tool and library that implements a variety of
construction algorithms for highly uniform point sets. 

The user can quickly discover the functionalities of LatNet Builder without anything to install using Binder TODO button. This service hosts a version of LatNet Builder which can be used through a Graphical User Interface. However, this service only provides limited computation ressources. For a better experience, one should directly install the software on their machine.

The simplest way to use the software is through its Graphical User Interface (GUI). 
A Command Line Interface (CLI) is available as an alternative.
The software can also be used as a C++ library. Interfaces with Python and R are also provided. Finally, the software can be used directly from the simulation software [SSJ](http://simul.iro.umontreal.ca/ssj/indexe.html).

LatNet Builder comprises two interdependent pieces called LatBuilder and NetBuilder.
*Lat Builder* focuses on the construction of rank-1 ordinary and polynomial lattice rules whereas *NetBuilder* supports the construction of digital nets in base 2. The quality of point sets are assessed using various measures of (non)uniformity of the points. 

The implemented quality measures include, for the LatBuilder part, the weighted P<sub>α</sub> square discrepancy, the R<sub>α</sub> criterion, and figures of merit based on the spectral test, and for the NetBuilder part, the weighted P<sub>2</sub> square discrepancy, the R<sub>1</sub> criterion and bit equidistribution quality measures such as the t-value and the resolution-gap.

All these measures, which are called figures of merit, are parametrized with weights to give different importances to the projections of the point sets.

The software supports exhaustive and random searches, as well as component-by-component
(CBC) and random CBC constructions, for any number of points, and for various
measures of (non)uniformity of the points.

The software also support the construction of multi-level point sets. A multi-level point set is actually a sequence of point sets whose points are embedded. Good multi-level point sets are highly uniform for their different embedding levels.

For modulus that are powers of an irreducible base, the construction
of embedded rank-1 ordinary and polynomial lattice rules is supported by LatBuilder through any of the above algorithms. In this case, a fast-CBC exploration is also available for specific figures of merit. For polynomial lattice rules, the fast-CBC algorithm is only available for irreducible modulus. 

NetBuilder supports the construction of various (finite) good digital sequences.

The software supports a variety of possibilities for the normalization of the merit values of individual embedded levels and for their combination into a single merit value.

#### Get the Software

- [**Download a binary release**] TODO
- [**Browse the source code**] TODO
#### Documentation

- [**Reference Publication**  
  *LatNet Builder*: A General Software Tool for Constructing Rank-1 Lattice Rules](http://www.iro.umontreal.ca/~lecuyer/myftp/papers/latnetbuilder.pdf)
- [**LatNet Builder Manual**](link) TODO


## Quick Start Guide

The easiest way to use LatNet Builder is to:

1. Install [Python](link to conda) (required for the web interface). TODO
2. [**Download a binary
   package**](link) corresponding
   to your platform 
3. Unpack the downloaded archive into a location of your choice.
   **No further installation is required.**
4. Navigate to the directory named `latnetbuilder` in the unpacked archive, then
   to the subdirectory named `bin`.
5. To use the
   [LatNet Builder GUI](#the-latnetbuilder-builder-graphical-user-interface), TODO
5. To use
   [LatNet Builder CLI](#the-latnetbuilder-builder-command-line-interface),
   invoke the `latnetbuilder` (or `latnetbuilder.exe` for Windows users) program with
   appropriate arguments.


## Binary Releases

[Binary (pre-compiled) releases](link)
of LatNet Builder are available for ??? platforms.
These include the executable `latnetbuilder` program, library and documentation,
together with the Graphical User Interface.

Interfaces with Python and R ???

The binary distribution packages, under the `latnetbuilder` base directory, have
the following directory structure:

* `bin`: contains the executable `latnetbuilder` program
  ([the Command Line Interface](#the-latnetbuilder-builder-command-line-interface))
  and the `latnetbuilder-gui.ipynb` Jupyter notebook (that launches
  [the Graphical User Interface](#the-latnetbuilder-builder-graphical-user-interface));
* `include`: contains the C++ header files necessary to use the LatNet Builder
  library;
* `lib`: contains the binary LatNet Builder library;
* `share/latnetbuilder/html`: contains the HTML documentation;
* `share/latnetbuilder/examples`: contains examples on using the LatNet Builder library.

## Compiling the Source Code

### Software Dependencies

Compiling LatNet Builder requires the following softwares to be installed on
the system:

* [Python](link) TODO
* [Boost C++ Libraries](http://www.boost.org/) 1.57.0 or later
  <small>
  Installation instructions (**section 5 is important**):
  [for Linux / MacOS](http://www.boost.org/doc/libs/release/more/getting_started/unix-variants.html),
  [for Microsoft Windows](http://www.boost.org/doc/libs/release/more/getting_started/windows.html)
  </small>
* [NTL](http://www.shoup.net/ntl/index.html) 10.4.0 or later
* [GMP](http://www.shoup.net/ntl/index.html) compatible version with your NTL installation
* [FFTW](http://fftw.org/) 3.3.4 or later
* [Git](http://git-scm.com/) *(optional for downloading the source code)*
* [Doxygen](http://www.stack.nl/~dimitri/doxygen/) *(optional for generating
  the documentation)*

You will also need a recent compiler compliant with the C++14 standard.
LatNet Builder is **known to compile** using:

* [GCC](http://gcc.gnu.org/) 4.8 or 4.9 on Linux ???
* [clang](http://clang.llvm.org/) 3.6.0 on Linux ???
* Apple LLVM version 6.0 (based on LLVM 3.5svn) on MacOS ???

### Obtaining the Source Code

Get the latest source tree from GitHub, either by typing:

	git clone --recursive https://github.com/umontreal-simul/latnetbuilder.git

If [Git](http://git-scm.com/) is not available on your system, you can click on
the [Download ZIP](https://github.com/umontreal-simul/latnetbuilder/archive/master.zip)
link from the [LatNet Builder GitHub
page](https://github.com/umontreal-simul/latnetbuilder), then by unzipping the downloaded
archive.

### Configuring the Build

LatNet Builder relies on the
[waf meta build system](https://code.google.com/p/waf/) for configuring and
compiling the software source.
Waf is included in the LatNet Builder source tree, but it depends on
[Python](http://python.org/download), which must be available on the system
on which LatNet Builder is to be compiled.

The commands below should work verbatim under Linux and MacOS systems.
**Microsoft Windows** users should replace every instance of `./waf` 
with the path under which the Python executable
(`python.exe`) or simply with `python waf`
if the Python installation path is accessible from the system `%PATH%`
environment variable.

Change the current directory to the root directory of the package, for example:

	cd latnetbuilder

if you obtained the source code with the `git` command.
If you obtained the source code from the ZIP archive, the directory should be
named `latnetbuilder-master` instead of `latnetbuilder`.
At the root of the source tree lies the `waf` script, manages the build
process.

Try:

	./waf --help

to see the various commands and options.
The most relevant options include `--out` to specify the directory in which the
files created during the build process will be placed, `--prefix` to specify
the directory under which you wish to install the LatNet Builder software
**once it is compiled**, and `--boost`, `--ntl`, `--gmp` and `--fftw` to specify the directories
under which Boost, NTL, GMP and FFTW were installed, if not under standard system
directories.  First, the project must be configured with:

	./waf configure --prefix $HOME/latnetsoft

with `$HOME/latnetsoft` replaced with the directory into which you wish to install
LatNet Builder.
Here, `$HOME` will expand to your own home directory; you can specify any other
directory to which you have permissions for write access, e.g., with `--prefix
/tmp/latnetsoft`.

If Boost, NTL, GMP and FFTW are not part of the standard system installation and were
manually installed under, say, the `/opt/boost`, `/opt/ntl`, `/opt/gmp` and `/opt/fftw` directories —
which means that `/opt/boost`, `/opt/ntl`, `/opt/gmp` and `/opt/fftw` both contain subdirectories named
`include` and `lib` — the following command indicates `waf` where to find these
two libraries:

	./waf configure --prefix $HOME/latnetsoft --boost /opt/boost --fftw /opt/fftw configure --link-static

The `--link-static` option suggested above will cause the 
libraries to be linked statically to the executable program, which may be
desirable especially if these are not installed in standard locations.

It is possible to set the `CXX` environment variable to the path to a specific
C++ compiler to be used to build LatNet Builder, before running the `waf
configure` command.

The above `waf configure` commands configures `waf` for a minimal build,
without documentation nor code examples.  These can be built by
appending the following options to `waf configure`:

* `--build-docs` to generate the documentation, if
  [Doxygen](http://www.stack.nl/~dimitri/doxygen/) is available on the system.
* `--build-examples` to compile and install example code, including
  code from the tutorial, which will also be verified to yield correct output.
  The expected outputs are stored in text files with names matching those of
  programs, under the `examples/tutorial/output` subdirectory.

Errors will be reported if required software components cannot be found.  In
that case, you should check the dependencies installation paths.

### Building and Installing

Once everything is configured correctly, the following command will build the
LatNet Builder library and command-line tool:

	./waf build

If the build process completed without errors, LatNet Builder can be installed
to `$HOME/latnetsoft`, or any directory specified with the `--prefix` options
during the configuration step, with:

	./waf install

The LatNet Builder executable can be found at `$HOME/latnetsoft/bin/latnetbuilder`
(with an additional `.exe` extension under Windows systems).

Before executing the LatNet Builder program, it may be necessary to
to add the paths to the Boost, NTL, GMP and FFTW libraries to the `LD_LIBRARY_PATH` (for
Linux) or to the `DYLD_FALLBACK_LIBRARY_PATH` (for MacOS) environment
variables, especially if you did not use the `--link-static` option on `waf
configure`.
For example, if you're using an `sh`-compatible shell and Boost, NTL, GMP and FFTW were
manually installed under the `/opt/boost`, `/opt/ntl`, `/opt/gmp` and `/opt/fftw` directories,
respectively, type:

	export LD_LIBRARY_PATH=/opt/boost/lib:/opt/ntl/lib:/opt/gmp/lib:/opt/fftw/lib:$LD_LIBRARY_PATH

under Linux, or

	export DYLD_FALLBACK_LIBRARY_PATH=/opt/boost/lib:/opt/ntl/lib:/opt/gmp/lib:/opt/fftw/lib:$LD_LIBRARY_PATH

under MacOS.

**Microsoft Windows** users might need to copy the Boost, NTL and GMP FFTW DLLs into the
same directory (`$HOME/latnetsoft/bin`, for example) as the `latnetbuilder`
executable program.

To check that the program installed correctly, run:

	$HOME/latnetsoft/bin/latnetbuilder --version

which should report the current LatNet Builder version.
Help on usage can be obtained by replacing the `--version` switch with the
`--help` switch.


## Using LatNet Builder

### The LatNet Builder Command-Line Interface

Information on using the LatNet Builder program is given in the
[LatNet Builder command line tutorial](link) TODO
that can also be found in `TODO`.
Compact usage information can also be obtained with `bin/latnetbuilder --help`.

The below command line assumes that the current working directory is that which
contains the `bin` subdirectory.  If you installed a binary package, it is the
`latnetbuilder` directory at the root of the package.

**Microsoft Windows** users should replace `bin/latnetbuilder` with
`bin\latnetbuilder.exe` in what follows.

#### Constructing rank-1 lattice rules

As a concrete example of how the program can be used, to construct, using the component-by-component (CBC) algorithm, a rank-1 ordinary
lattice rule with 8191 points in 5 dimensions using the P<sub>α</sub> criterion with
$\alpha = 2$ and with uniform product weights with value 0.8, issue the following
command (from the installation directory):

	bin/latnetbuilder -T lattice -c ordinary -s 8191 -d 5 -M CU:P2 -w product:0.8 -E CBC

#### Constructing digital nets

Likewise, to construct, using the component-by-component (CBC) algorithm, Sobol' net with 2<sub>10</sub> points in 10 dimensions using the P<sub>α</sub> criterion with
$\alpha = 2$ and with uniform product weights with value 0.8, issue the following
command (from the installation directory):

  bin/latnetbuilder -T net -c sobol -s 2^10 -d 10 -a CU:P2/2/2/product:0.8 -E random:1000


### The LatNet Builder Graphical User Interface

The LatNet Builder Graphical User Interface is included in the binary packages.
[Python](http://python.org/download/) must be installed on the host machine.
A connection to the Internet is also necessary to display the mathematical
symbols.
The local web server can be started by typing the following commands in a terminal
  source activate latnetbuilder
	jupyter notebook latnetbuilder-gui.ipynb

It requires the LatNet Builder program above to be working properly.

**Microsoft Windows** users should replace TODO

If you have a network connection, the LatNet Builder GUI may also be accessed through the Binder web service without installing anything on your computer. Note that the computation ressources could be limited.

TODO screenshot + link binder


### The LatNet Builder Library

#### Understanding the API

Several examples of code using the LatNet Builder application programming
interface (API) can be found under the `share/doc/examples`
directory under the installation directory and in subdirectories.

To teach yourself how to code using the LatNet Builder library, you can read:

* [Library Tutorial](http://simul.iro.umontreal.ca/latnetbuilder/doc/da/d6f/libtut.html)

#### Compiling and Linking

Compiling and linking code with the LatNet Builder library requires the same
[software dependencies](#software-dependencies) to be available as for
compiling the LatNet Builder program itself.

External software can make use of the LatNet Builder libraries by
setting the compiler to use the C++14 standard, by adding the `include`
and `lib` directories (under the installation directory) to the include and
library paths, respectively, then by linking with the `latticetester` and
`latnetbuilder` libraries, together with the `fftw3`, NTL, GMP and Boost libraries.
An example `Makefile` to build a project that uses the LatNet Builder
application programming interface (API) can be found under
`$HOME/latnetsoft/share/doc/examples/myproject`. TODO
If you compiled LatNet Builder yourself from the source code, this `Makefile`
will be adapted to your specific system configuration; otherwise, it may need
to be customized. TODO

In particular, if LatNet Builder, Boost , NTL, GMP and FFTW were respectively installed
under `$HOME/latnetsoft`, `/opt/boost`, `/opt/ntl`, `/opt/gmp` and `/opt/FFTW`, a C++ source file called
`myprog.cc` can be compiled and linked into an executable called `myprog` by
using the following command line with GCC: TODO

	g++ -std=c++14 -I$HOME/latnetsoft/include -I/opt/boost/include -I/opt/ntl/include -I/opt/gmp/include -I/opt/fftw/include -O2 -o myprog myprog.cc -L$HOME/latnetsoft/lib -l latnetbuilder -l latticetester -L/opt/ntl/lib  -l ntl -L/opt/gmp/lib  -l gmp -L/opt/fftw/lib -l fftw3

With clang, just replace `g++` with `clang++`.


#### Using LatNet Builder with Xcode 6.0

To create an Xcode project that uses LatNet Builder, follow the following
steps:

1. Create a new Xcode projet as: **Application &#9658; Command Line Tool**
2. Once the Xcode project displays, select the application name (blue file) on the left pane.
3. Set **Build Settings &#9658; Search Paths &#9658; Always Search User Paths** to **Yes**.
4. To **Build Settings &#9658; Search Paths &#9658; User Header Search Paths**, add the path
   to the `include` directory from your LatNet Builder installation (e.g.,
   `/Users/me/latnetbuilder/include`) and the paths to the `include` directories
   from your Boost, NTL, GMP and FFTW installations if not under standard system
   locations.
5. To **Build Phases &#9658; Link Binary With Libraries**, add (+) the following files:
     - `liblatnetbuilder.a` and `liblatticetester.a ` from the `lib` directory of your
       LatNet Builder installation (e.g., `/Users/me/latnetbuilder/lib`);
     - `libntl.a` from your NTL installation.
     - `libgmp.a` from your GMP installation.
     - `libfftw3.a` from your FFTW installation.


## Frequently Asked Questions

TODO

### What should I use to find good high-dimensional lattices in reasonable time?

Fast CBC with a coordinate-uniform figure of merit.

For example, the following command line performs a fast CBC exploration using
the weighted P<sub>α</sub> criterion with α=2 and with product weights giving
the same weight of 0.1 to every coordinate (this means a weight of
10<sup>-2</sup> for projections of order 1, of 10<sup>-4</sup> for projections
of order 2, of 10<sup>-6</sup> for projections of order 3, etc.):

	./latnetbuilder -T lattice -c ordinary -s 2^16 -d 100 -M CU:P2 -w product:0.1 -E fast-CBC

The above search is for n=2<sup>16</sup>=65,536 points in dimension 100.  LatNet Builder
does that very quickly.
It may be necessary to enclose some arguments in double quotes, depending on
the shell (command line interpreter).

#TODO
binder
docker
conda + python deps
host the doc somewhere
links
releases