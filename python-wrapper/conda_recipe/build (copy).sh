#!/bin/bash

export WORK_DIR=$(pwd)

cd $WORK_DIR/gmp-6.1.2
./configure --prefix=$HOME/dependencies
make
make install

cd $WORK_DIR/ntl-11.0.0/src
./configure PREFIX=$HOME/dependencies GMP_PREFIX=$HOME/dependencies
make
make install

cd $WORK_DIR/fftw-3.3.8
./configure --prefix=$HOME/dependencies
make
make install

cd $WORK_DIR/boost_1_67_0
./bootstrap.sh --prefix=$HOME/dependencies --with-libraries=program_options
./b2 install

cd $WORK_DIR/latbuilder
./waf configure --prefix $PREFIX/latsoft --boost $HOME/dependencies --fftw $HOME/dependencies --ntl $HOME/dependencies --link-static
./waf build
./waf install
