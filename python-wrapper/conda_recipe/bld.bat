@echo off
python -m pip install --no-deps --ignore-installed python-wrapper\.
REM python setup.py install --single-version-externally-managed --record record.txt
if errorlevel 1 exit 1

echo F | xcopy /Y python-wrapper\windows_docker\latnetbuilder_windows.py %PREFIX%\share\latnetbuilder\latnetbuilder_windows.py
if errorlevel 1 exit 1

echo F | xcopy /Y python-wrapper\windows_docker\latnetbuilder.bat %PREFIX%\bin\latnetbuilder.bat
if errorlevel 1 exit 1

echo F | xcopy /Y python-wrapper\windows_docker\latnetbuilder_configure.bat %PREFIX%\bin\latnetbuilder_configure.bat
if errorlevel 1 exit 1

echo F | xcopy /Y python-wrapper\windows_docker\configure.py %PREFIX%\share\latnetbuilder\configure.py
if errorlevel 1 exit 1