#!/bin/bash

export WORK_DIR=$(pwd)
echo $WITHOUT_WAF

python -m pip install --no-deps --ignore-installed python-wrapper/.

if [ WITHOUT_WAF!=1 ]
then
    cd $WORK_DIR
    ./waf configure --prefix $PREFIX --link-static
    ./waf build
    ./waf install
fi