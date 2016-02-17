@echo off
Title Building Smokediff for OSX

::  Windows batch file to build OSX version of smokediff

:: setup environment variables (defining where repository resides etc) 

set envfile="%userprofile%"\fds_smv_env.bat
IF EXIST %envfile% GOTO endif_envexist
echo ***Fatal error.  The environment setup file %envfile% does not exist. 
echo Create a file named %envfile% and use SMV/scripts/fds_smv_env_template.bat
echo as an example.
echo.
echo Aborting now...
pause>NUL
goto:eof

:endif_envexist

call %envfile%

%svn_drive%
cd %svn_root%\smv\scripts

set scriptdir=%linux_svn_root%/SMV/scripts
echo Building OSX version of smokediff

plink %osx_logon% %scriptdir%/ssh_command.csh %osx_hostname% %scriptdir% MAKEsmdosx.csh

echo.
echo compilation complete
pause
