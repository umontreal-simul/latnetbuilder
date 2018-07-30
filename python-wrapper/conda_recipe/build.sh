#!/bin/bash

python -m pip install --no-deps --ignore-installed python-wrapper/.

if [ WITHOUT_WAF!=1 ]
then
    cd $LATNETBUILDER_INSTALL_DIR
    cp -R . $PREFIX
fi