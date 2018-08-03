python -m pip install --no-deps --ignore-installed python-wrapper\.

xcopy /s python-wrapper\windows_docker\latnetbuilder.py $PREFIX\share\latnetbuilder\latnetbuilder.py
xcopy /s python-wrapper\windows_docker $PREFIX\bin\latnetbuilder /O /K