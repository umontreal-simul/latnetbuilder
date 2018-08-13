@echo off
python %~dp0%..\share\latnetbuilder\configure.py
pause
if errorlevel 1 exit 1

