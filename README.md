# LatNet Builder
_A general software tool for constructing highly uniform point sets_

[**LatNet Builder Manual**](http://umontreal-simul.github.io/latnetbuilder/)

**Outline**:
1) [A software for quasi-Monte Carlo](#a-software-for-quasi-monte-carlo)
2) [How to use the software](#how-to-use-the-software)
3) [How to get the software](#how-to-get-the-software)
4) [For developers](#for-developers)
5) [Further information](#further-information)

## A software for quasi-Monte Carlo

*LatNet Builder* is a C++ software library and tool for constructing *highly-uniform point sets* for *quasi-Monte Carlo* 
and *randomized quasi-Monte Carlo* methods using state-of-the-art techniques.

Quasi-Monte Carlo methods are often used as a replacement for [*Monte
Carlo*](http://en.wikipedia.org/wiki/Monte_Carlo_integration) to integrate
multidimensional functions.

Monte Carlo and quasi-Monte Carlo methods are used for the same purpose. The problem is to approximate the integral of a function over the unit hypercube as the average of the function evaluated at a set of points. Whereas Monte-Carlo uses a (pseudo)-random sequence of points, quasi-Monte Carlo uses a highly uniform point set, that is a point set with low *discrepancy*.
For certain problems, well-constructed point sets can dramatically reduce the integration error with respect to Monte Carlo.

LatNet Builder implements a variety of
construction algorithms for highly uniform point sets. The point set types handled by LatNet Builder include rank-1 ordinary and polynomial lattice rules and digital nets in base 2, including Sobol' nets.

More precisely, LatNet Builder can search for point sets with an arbitrary number of points in any dimension of high quality with respect to various criteria. 
Such quality criteria are called *figures of merit*. These figures of merit are parametrized by *weights* which give different importance to the subprojections of the net. 
The merits of the subprojections are aggregated using a (weighted) *𝓁<sub>q</sub> norm*.
LatNet Builder can use various *exploration methods* to construct the point sets, such as the exhaustive, the random sampling or the component-by-component (CBC) methods.

Additionally LatNet Builder contains more advanced features such as multilevel point sets, extensible point sets, interlaced point sets,
normalizations, and filters.

The features of LatNet Builder are summed up in the following table:

<table border="1" align="center" class="tg">
  <tr align="center">
    <th class="tg-uys7">Features</th>
    <th class="tg-uys7" colspan="2">Lattice rules</th>
    <th class="tg-uys7">Digital nets</th>
  </tr>
  <tr align="center">
    <td class="tg-uys7"> Point set types </td>
    <td class="tg-uys7">Rank-1 ordinary lattice rules</td>
    <td class="tg-uys7">Rank-1 polynomial lattice rules</td>
    <td class="tg-uys7">Sobol' nets<br> Polynomial lattice rules<br>Nets with explicit generating matrices</td>
  </tr>
  <tr align="center">
    <td class="tg-uys7"> Figures of merit </td>
    <td class="tg-uys7">P<sub>𝛼</sub>, R<sub>𝛼</sub>, spectral test</td>
    <td class="tg-uys7">P<sub>𝛼</sub>, R</td>
    <td class="tg-uys7">P<sub>𝛼</sub>, R, t-value, resolution-gap</td>
  </tr>
  <tr align="center">
    <td class="tg-uys7"> Types of weights </td>
    <td class="tg-uys7" colspan="3">projection-dependent, order-dependent, product, product-order-dependent and combined weights</td>
  </tr>
  <tr align="center">
    <td class="tg-uys7"> Exploration methods </td>
    <td class="tg-uys7" colspan="3">evaluation, exhaustive, random, full CBC and random CBC, fast CBC</td>
  </tr>
  <tr align="center">
    <td class="tg-uys7"> Multilevel point sets </td>
    <td class="tg-uys7" colspan="2">Embedded lattices</td>
    <td class="tg-uys7">Digital sequences</td>
  </tr>
  <tr align="center">
    <td class="tg-uys7"> Additional features</td>
    <td class="tg-uys7"> Extensible lattices, normalizations and filters</td>
    <td class="tg-uys7">Extensible lattices, normalizations and filters, interlaced polynomial lattice rules</td>
    <td class="tg-uys7">Interlaced digital nets</td>
  </tr>
</table> 

## How to use the software

### Quick overview
The user can quickly discover the functionalities of LatNet Builder without anything to install using [Binder](https://mybinder.org/): 

[![Binder](https://mybinder.org/badge.svg)](https://mybinder.org/v2/gh/umontreal-simul/latnetbuilder/master?urlpath=/apps%2Fpython-wrapper%2Fnotebooks%2FInterface.ipynb)

This service hosts a version of LatNet Builder which can be used through a Graphical User Interface (GUI). However, this service only provides limited computation resources. For a better experience, one should install the software on their machine.


### Main components
LatNet Builder offers various possibilities to use its functionalities:  

- a C++ library
- a Command Line Tool
- a Python package which comprises:
  - a [Python interface](python-wrapper/README.md)
  - a Graphical User Interface based on the [Jupyter](https://jupyter.org) ecosystem
- a Java interface encapsulated in the [Stochastic Simulation in Java (SSJ) software](https://github.com/umontreal-simul/ssj)

There are four ways to get the software: 
- [downloading a binary release](#downloading-a-binary-release) (only available for Linux and Mac OS)
- [installing the Python package](#installing-with-conda) (with conda)
- [using a Docker container](#using-a-docker-container), a 'light-weight virtual machine'
- [compiling the source code](#compiling-the-source-code)

The binary pre-compiled release of LatNet Builder is available on itself and also wrapped in a Python package. The latter provides a one-line installation procedure for the software, its GUI and its Python interface. 

The Docker container, which encapsulates a Linux distribution, is meant as
a convenience for Windows users. As an additional convenience, the Python package in its Windows version acts as a wrapper above the Docker container. Users unfamiliar with Docker should thus consider using the Python package.

The source code should compile fine on Linux and Mac OS platforms. However, since a lot of dependencies are required, we advise you to use the binary pre-compiled release available either on its own or through the Python package. 

For Windows users who would like to compile the source code, we recommend to use a UNIX emulation environment like Cygwin or MinGW, instead of Microsoft development tools. Compiling LatNet Builder and its dependencies may turn out to be time-consuming so we highly recommend to use the conda package or directly the Docker container.

### The command-line tool in a nutshell

Information on using the LatNet Builder program is given in the
[LatNet Builder command line tutorial](http://umontreal-simul.github.io/latnetbuilder/d5/d56/cmdtut.html).
Compact usage information can also be obtained with `latnetbuilder --help`.

The below command line assumes that the `latnetbuilder` executable was added to your PATH environment variable. It will be the case if you installed LatNet Builder with conda or are using a Docker container. If you installed a binary package, you must replace
`latnetbuilder` with a path to the `latnetbuilder` executable. For instance if you installed LatNet Builder in `$HOME/latnetsoft`, use `$HOME/latnetsoft/bin/latnetbuilder`.

#### Constructing rank-1 lattice rules

As a concrete example of how the program can be used, the following command line performs a fast CBC exploration using
the weighted P<sub>α</sub> criterion with α=2 and with product weights giving
the same weight of 0.1 to every coordinate (this means a weight of
10<sup>-2</sup> for projections of order 1, of 10<sup>-4</sup> for projections
of order 2, of 10<sup>-6</sup> for projections of order 3, etc.):

```
latnetbuilder -t lattice -c ordinary -s 2^16 -d 100 -f CU:P2 -q 2 -w product:0.1 -e fast-CBC
```

The above search is for n=2<sup>16</sup>=65,536 points in dimension 100. LatNet Builder
does that in less than 2 seconds.

#### Constructing digital nets

Likewise, to construct a Sobol' net with 2<sup>16</sup> points in 10 dimensions using the random component-by-component (CBC) algorithm, with 70 samples per coordinate and a t-value-based criterion with order-dependent
weights equal to 1 on the 2-dimensional and 3-dimensional projections, issue the following
command:

```
latnetbuilder -t net -c sobol -s 2^16 -d 10 -f projdep:t-value -q inf -w order-dependent:0:0,1,1 -e random-CBC:70
```

### The LatNet Builder library 

Several examples of code using the LatNet Builder application programming
interface (API) can be found under the `share/doc/examples`
directory under the installation directory and in subdirectories.

To learn how to code using the LatNet Builder library, you can read the [Library Tutorial](http://umontreal-simul.github.io/latnetbuilder/da/d6f/libtut.html).

Compiling and linking code with the LatNet Builder library requires the same
[software dependencies](#software-dependencies) to be available as for
compiling the LatNet Builder program itself.

External software can make use of the LatNet Builder libraries by
setting the compiler to use the C++14 standard, by adding the `include`
and `lib` directories (under the installation directory) to the include and
library paths, respectively, then by linking with the `latticetester` and
`latnetbuilder` libraries, together with the `fftw3`, NTL, GMP and Boost libraries.

In particular, if LatNet Builder, Boost , NTL, GMP and FFTW were respectively installed
under `$HOME/latnetsoft`, `/opt/boost`, `/opt/ntl`, `/opt/gmp` and `/opt/FFTW`, a C++ source file called
`myprog.cc` can be compiled and linked into an executable called `myprog` by
using the following command line with g++:
```bash
g++ 
  -std=c++14 -O2 \
  -I$HOME/latnetsoft/include \
  -I/opt/boost/include \
  -I/opt/ntl/include \
  -I/opt/gmp/include \
  -I/opt/fftw/include \
  -o myprog myprog.cc \ 
  -L$HOME/latnetsoft/lib -l latnetbuilder -l latticetester \
  -L/opt/ntl/lib  -l ntl \
  -L/opt/gmp/lib  -l gmp \
  -L/opt/fftw/lib -l fftw3 \
  -L /opt/boost/lib -l boost_program_options -l boost_system -l boost_filesystem
```

With clang, just replace `g++` with `clang++`.

## How to get the software

### Downloading a binary release

[Binary pre-compiled releases](https://github.com/umontreal-simul/latnetbuilder/releases)
of LatNet Builder are available **for Linux and Mac OS platforms**.
These include the executable `latnetbuilder` program, library and documentation.

The binary distribution packages, under the `latnetbuilder` base directory, have the following directory structure:

* `bin` contains the executable `latnetbuilder` program (the command-line tool)
* `include` contains the C++ header files necessary to use the LatNet Builder library
* `lib` contains the LatNet Builder and LatticeTester libraries (LatticeTester is an embedded dependency of the LatNet Builder library)
* `share/doc/latnetbuilder/` contains the HTML documentation
* `share/doc/latnetbuilder/examples` contains examples on using the LatNet Builder library
* `share/latnetbuilder` and `share/latticetester` contain some data files used by the libraries

### Installing with conda

[![Anaconda-Server Badge](https://anaconda.org/umontreal-simul/latnetbuilder/badges/installer/conda.svg)](https://conda.anaconda.org/umontreal-simul)

The Python package, which contains the [binary pre-compiled release](#downloading-a-binary-release) plus the GUI, must be installed using [Conda](https://conda.io/docs/). Conda is an open source package management system and environment management system, which is very popular in the Python community. 

First, you have to download Conda from the [Anaconda distribution](https://www.anaconda.com/download).
For more information, see the [conda documentation about environments](https://conda.io/docs/user-guide/tasks/manage-environments.html).

The Conda `latnetbuilder` package is available **for Windows, Linux and Mac OS.** The Windows version acts as a wrapper above a Docker container. 

The installation procedures is as follow:

#### For Linux and MacOS

After installing Conda, you can run the following commands:

```bash
conda create -n latnetbuilder   # create a conda environment named latnetbuilder
conda config --add channels conda-forge         # add conda-forge as a default conda channel
conda install -c umontreal-simul latnetbuilder   # installs the latnetbuilder package from the channel umontreal-simul
```

Temporary fix of [issue 8](https://github.com/umontreal-simul/latnetbuilder/issues/8): On MacOS, please specify the version of the package, as follows

```bash
conda install -c umontreal-simul latnetbuilder=2.1.1
```

#### For Windows

You have to install Docker first. [Docker](https://docs.docker.com/) provides a way to run applications securely isolated in a container, packaged with all its dependencies and libraries. It is similar to a virtual machine. Depending on your Windows version, as of now, two Docker installations are available:
+ for Windows 10 Professional or Enterprise 64-bit users: [Docker for Windows](https://docs.docker.com/docker-for-windows/install/#start-docker-for-windows)
+ for all other users: [Docker Toolbox](https://docs.docker.com/toolbox/toolbox_install_windows/)

In either case, if you are given the choice during the installation procedure, please add Docker to your PATH.
Else, the configuration of LatNet Builder will require you to state the path to your Docker installation, so please remember it when you set it during the installation procedure.

The installation of Docker can be tricky, so please follow precisely the instructions on the Docker website and double-check that your installation works before moving on to the installation of LatNet Builder.

After installing Docker and Conda, you can run the following commands to install LatNet Builder: 

```bash
conda create -n latnetbuilder   # create a conda environment named latnetbuilder
conda config --add channels conda-forge         # add conda-forge as a default conda channel
conda install -c umontreal-simul latnetbuilder   # installs the latnetbuilder package from the channel umontreal-simul
activate latnetbuilder  # activate the conda environment named latnetbuilder
latnetbuilder_configure # run the configuration script (which will locate the Docker installation on your system)
```

#### Running LatNetBuilder and its interface (all systems)

```bash
source activate latnetbuilder  # for UNIX
activate latnetbuilder  # for Windows
# you can call latnetbuilder from the command line:
latnetbuilder --help
# you can also start the interface:
latnetbuilder-gui
```

This latest command will open a Jupyter notebook in your browser. 
Click on the AppMode button to run the GUI in full-screen mode. For more information about Jupyter notebooks, you can have a look at the [Jupyter documentation](http://jupyter-notebook-beginner-guide.readthedocs.io/en/latest/execute.html).

Remark: for Windows users with Docker Toolbox, the LatNet Builder package creates and manipulates a virtual machine. It is possible that when you shut your system down, it complains about a running VirtualBox application. This is because the LatNet Builder package does not destroy the virtual machine it creates. You can either force the shutdown of your system or destroy the machine yourself with the command `docker-machine rm latnetbuilder`. The next time you use LatNet Builder, the machine will be re-created.

### Using a Docker container

[Docker](https://docs.docker.com/) provides a way to run applications securely isolated in a container, packaged with all its dependencies and libraries. It is similar to a virtual machine.

Two Docker images are available:
- a light (446 MB) image which runs on a Linux system the binary pre-compiled executable
- a complete (1.6 GB) image which additionally contains the GUI


To install LatNetBuilder and its interface:
+ [install Docker](https://docs.docker.com/install/)
+ pull the image:

```bash
docker pull umontrealsimul/latnetbuilder:complete    # for the complete image
docker pull umontrealsimul/latnetbuilder:light    # for the light image
```

To run LatNetBuilder and its interface: the following command starts a console inside a Docker container and tells your system to listen at port 8888 to the port 8888 of the Docker container.

```bash
docker run -it -p 8888:8888 umontrealsimul/latnetbuilder:complete bash   # for the complete image
docker run -it umontrealsimul/latnetbuilder:light bash   # for the light image
```

Then you can call LatNet Builder from the command line:

```bash
latnetbuilder --help
```

With the complete image, you can also start the interface. The following command starts a Jupyter server inside the Docker container which emits in port 8888. Once this is done, you can browse to `localhost:8888` in your browser, enter the password (`latnet`), start the `Interface.ipynb` notebook, and finally click on the 'Appmode' button to start the GUI App.

**Again, the password is `latnet`**. It is only meant to secure the Jupyter server which is only useful for network applications.

```bash
latnetbuilder-gui-docker
```

Once you are done, do not forget to stop the Jupyter server inside the container. Even when you close the console window, the container is still running. You can see the list of running containers (and their names) with the command

```bash
docker container ls
```

You can either leave it running or stop it. To stop the container:
```bash
docker stop CONTAINER_NAME
```

If you leave it running, to start a new console you can use the following command instead of the `docker run` command above:

```bash
docker exec -it CONTAINER_NAME bash
```

### Compiling the source code

This method works fine for Linux and Mac OS X users. For Windows users, the software is known to compile on Cygwin, without the `--link-static` option. Nevertheless, this method is really complex and we recommended you avoid it, unless you are a power user.

#### Software Dependencies

Compiling LatNet Builder requires the following softwares to be installed on
the system:

* [Python](https://www.python.org/getit/) 3.6 or later
* [Boost C++ Libraries](http://www.boost.org/doc/libs/release/more/getting_started/unix-variants.html) 1.67.0 or later
* [NTL](http://www.shoup.net/ntl/index.html) 11.0.0 or later
* [GMP](https://gmplib.org/#DOWNLOAD) compatible version with your NTL installation
* [FFTW](http://fftw.org/) 3.3.4 or later
* [Git](http://git-scm.com/) *(optional for downloading the source code)*
* [Doxygen](http://www.stack.nl/~dimitri/doxygen/) *(optional for generating
  the documentation)*

You will also need a recent compiler compliant with the C++14 standard.
LatNet Builder is **known to compile** using:

* [GCC](http://gcc.gnu.org/) 5.4.0 on Linux
* Apple LLVM version 9.1.0 on MacOS

#### Obtaining the Source Code

Get the latest source tree from GitHub by typing:

	git clone --recurse-submodules https://github.com/umontreal-simul/latnetbuilder.git

The `--recurse-submodules` clones at the same time LatticeTester, which is a subrepository of LatNet Builder.

If [Git](http://git-scm.com/) is not available on your system, you can click on
the [Download ZIP](https://github.com/umontreal-simul/latnetbuilder/archive/master.zip)
link from the [LatNet Builder GitHub
page](https://github.com/umontreal-simul/latnetbuilder), then by unzipping the downloaded
archive.
In this case, the `latticetester` folder is empty, so you need to also [download a ZIP archive of LatticeTester](https://github.com/umontreal-simul/latticetester/archive/master.zip), unzip it and copy the contents inside the `latticetester` folder.

#### Configuring the Build

LatNet Builder relies on the
[waf meta build system](https://gitlab.com/ita1024/waf/) for configuring and
compiling the software source.
Waf is included in the LatNet Builder source tree, but it depends on
[Python](https://www.python.org/getit/), which must be available on the system
on which LatNet Builder is to be compiled.

The commands below should work verbatim under Linux and MacOS systems.

Change the current directory to the root directory of the package, for example:

	cd latnetbuilder

if you obtained the source code with the `git` command.
If you obtained the source code from the ZIP archive, the directory should be named `latnetbuilder-master` instead of `latnetbuilder`.
At the root of the source tree lies the `waf` script which manages the build process.

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

with `$HOME/latnetsoft` replaced with the directory into which you wish to install LatNet Builder.
Here, `$HOME` will expand to your own home directory; you can specify any other
directory to which you have permissions for write access, e.g., with `--prefix
/tmp/latnetsoft`.

If Boost, NTL, GMP and FFTW are not part of the standard system installation and were
manually installed under, say, the `/opt/boost`, `/opt/ntl`, `/opt/gmp` and `/opt/fftw` directories —
which means that `/opt/boost`, `/opt/ntl`, `/opt/gmp` and `/opt/fftw` both contain subdirectories named
`include` and `lib` — the following command indicates `waf` where to find these
two libraries:

	./waf configure --prefix $HOME/latnetsoft --boost /opt/boost --ntl /opt/ntl --gmp /opt/gmp --fftw /opt/fftw configure --link-static

The `--link-static` option suggested above will cause the 
libraries to be linked statically to the executable program, which may be
desirable especially if these are not installed in standard locations.

It is possible to set the `CXX` environment variable to the path to a specific
C++ compiler to be used to build LatNet Builder, before running the `waf
configure` command.

The above `waf configure` commands configures `waf` for a minimal build,
without documentation, code examples nor GUI.  These can be built by
appending the following options to `waf configure`:

* `--build-docs` to generate the documentation, if
  [Doxygen](http://www.stack.nl/~dimitri/doxygen/) is available on the system.
  
* `--build-examples` to compile and install example code, including
  code from the tutorial, which will also be verified to yield correct output.
  The expected outputs are stored in text files with names matching those of
  programs, under the `examples/tutorial/output` subdirectory.

* `--build-conda` to build the Python package then install it in a [`latnetbuilder` conda environment](#installing-with-conda). More precisely, the package contains the LatNet Builder software and its Python interface. Thus, with this option, two versions of the software are installed: one in your installation folder, and one wrapped inside the Python package. 

Errors will be reported if required software components cannot be found.  In
that case, you should check the dependencies installation paths.

#### Building and Installing

Once everything is configured correctly, the following command will build the
LatNet Builder library and command-line tool:

	./waf build

If the build process completed without errors, LatNet Builder can be installed
to `$HOME/latnetsoft`, or any directory specified with the `--prefix` options
during the configuration step, with:

	./waf install

The LatNet Builder executable can be found at `$HOME/latnetsoft/bin/latnetbuilder`.

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

To check that the program installed correctly, run:

	$HOME/latnetsoft/bin/latnetbuilder --version

which should report the current LatNet Builder version.
Help on usage can be obtained by replacing the `--version` switch with the
`--help` switch.

## For developers

### Binary pre-compiled releases 

To compile a portable executable, you first need to compile a portable static library for each dependency (GMP, NTL, Boost and FFTW). If the libraries are somewhere in your path (typically in /usr/local/lib), we advise to remove them beforehand, to make sure dynamic libraries do not get linked by error.

The following commands **should** do the job. 
- If you compile on Linux:
	- replace `<OS>` by `linux`
  - add `-fPIC` to the list of CXXFLAGS and CFLAGS for FFTW, GMP, NTL
  - add `cxxflags=-fPIC cflags=-fPIC` to the ./b2 command for Boost
- If you compile on Mac OS X:
	- replace `<OS>` by `darwin`
	- add `--enable-assembly=no` to the configuration of GMP
	- replace `--with-toolset=gcc` by `--with-toolset=clang`
	- remove from the installation folder of Boost the shared libaries (.dylib) after installation

**FFTW**
```bash
export CXXFLAGS='-m64 -march=k8'
export CFLAGS='-m64 -march=k8'
./configure --prefix=$HOME/fftw --enable-static --enable-threads --with-combined-threads --enable-sse2
make
make install
```

**GMP**
```bash
export CXXFLAGS='-m64 -march=k8'
export CFLAGS='-m64 -march=k8'
./configure --prefix=$HOME/gmp --enable-shared=no --host=x86_64-unknown-<OS>-gnu
make
make install
```

**NTL**
```bash
./configure "CXXFLAGS=-O2 -m64 -march=k8 -std=c++11" GMP_PREFIX=$HOME/gmp PREFIX=$HOME/ntl NTL_THREADS=off
make
make install
```

**Boost**
```bash
./bootstrap.sh --prefix=$HOME/boost --with-toolset=gcc --without-icu --with-libraries=program_options,system,filesystem
./b2 -j4 --prefix=$HOME/boost --layout=tagged link=static threading=single variant=release target-os=<OS> address-model=64
./b2 install
```

**LatNet Builder**
```bash
./waf configure --prefix=$HOME/latnetsoft --fftw=$HOME/fftw --gmp=$HOME/gmp --ntl=$HOME/ntl --boost=$HOME/boost --link-static --build-examples --build-docs --build-conda
./waf build
./waf install
```

Publish a new release on the [release page](https://github.com/umontreal-simul/latnetbuilder/releases) and update the features below.

### Updating the documentation

See the [README](https://github.com/umontreal-simul/latnetbuilder/tree/gh-pages) file
on the gh-pages branch.

### Updating the Python package on Anaconda Cloud

See the [README](python-wrapper/README.md) file in the `python-wrapper` directory.

### Updating the Docker images on DockerHub:

To update the complete docker image, run from the source code directory:

```bash
docker login
docker build -t umontrealsimul/latnetbuilder:complete --no-cache -f DockerfileComplete .
docker push umontrealsimul/latnetbuilder:complete
```

To update the light docker image, run from the installation directory:

```bash
docker login
docker build -t umontrealsimul/latnetbuilder:light --no-cache -f share/latnetbuilder/DockerfileLight .
docker push umontrealsimul/latnetbuilder:light
```

### Maintaining the online GUI on Binder

Binder uses the GitHub repository to build the online GUI. More precisely, it uses the `environment.yml` file to build the online GUI: it downloads the latnetbuilder package from Anaconda Cloud, and creates a Docker image to run the `Interface.ipynb` notebook.

Therefore, to re-build the online GUI, you have to:
- first upload the new version of the package to Anaconda Cloud
- then , after a new push on the master branch, the first connection will trigger
a rebuild of the online GUI.


## Further information

### Publications

P. L'Ecuyer, P. Marion, M. Godin, and F. Puchhammer, _A Tool for Custom Construction of QMC and RQMC Point Sets_, Monte Carlo and Quasi-Monte Carlo Methods 2020, accepted for publication. [arXiv link.](https://arxiv.org/abs/2012.10263)

P. Marion, M. Godin, and P. L'Ecuyer, _An algorithm to compute the t-value of a digital net and of its projections_, Journal of Computational and Applied Mathematics, volume 371, paper 112669, June 2020. [arXiv link.](http://arxiv.org/abs/1910.02277)

P. L'Ecuyer and D. Munger, _Algorithm 958: LatticeBuilder: A General Software Tool for Constructing Rank-1 Lattice Rules_, ACM Transactions on Mathematical Software, 42, 2 (2016), Article 15. [Online access.](http://dl.acm.org/citation.cfm?id=2754929)

See also the `reports` folder for internship reports on LatNet Builder.

### Contributors

This project was supervised by Pierre L'Ecuyer in the Département d’Informatique et de Recherche
Opérationnelle at the Université de Montreal.

List of contributors (by alphabetical order):

+ Yocheved Darmon
+ Maxime Godin
+ Ayman Jemel
+ Pierre Marion
+ David Munger