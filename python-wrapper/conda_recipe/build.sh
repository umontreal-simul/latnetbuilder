#!/bin/bash

export WORK_DIR=$(pwd)

python -m pip install --no-deps --ignore-installed python-wrapper/.

cd $WORK_DIR
./waf configure --prefix $PREFIX --link-static
./waf build
./waf install
