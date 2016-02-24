@echo off

rem   first, try with Python 3

if exist %SystemRoot%\py.exe (
  start %SystemRoot%\py.exe latbuilder-web.py
  exit
)

rem   check if Python 2.7 is installed
set PYIK="HKLM\SOFTWARE\Python\PythonCore\2.7\InstallPath"
reg query %PYIK% /ve
if errorlevel 1 (
  if exist C:\Python27\python.exe (
    set pydir=C:\Python27
  ) else (
    echo Cannot find Python 2.7.  Please install from https://www.python.org/downloads/.
    pause
    exit 1
  )
) else (
  rem   extract Python installation path from registry
  for /f "skip=2 tokens=1,* delims=)" %%a in ('reg.exe query %PYIK% /ve') do (
    for /f "tokens=1,*" %%c in ("%%b") do (
      set pydir=%%d
    )
  )
)

start %pydir%\python.exe latbuilder-web.py
