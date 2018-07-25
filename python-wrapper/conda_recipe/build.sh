#!/bin/bash

python -m pip install --no-deps --ignore-installed python-wrapper/.

if [ WITHOUT_WAF!=1 ]
then
    cd $TOTO_VARIABLE_UNIQUE_NAME
    cp -R . $PREFIX
fi