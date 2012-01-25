@echo off
Title Building Smokeview for 64 bit Linuux

Rem  Windows batch file to build a release Smokeview for Linux 64.

Rem setup environment variables (defining where repository resides etc) 

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
set version=%smv_version%_%smv_revision%

set scriptdir=FDS-SMV/SMV/scripts
set bundledir=FDS-SMV/SMV/for_bundle
set bindir=FDS-SMV/SMV/bin

plink %svn_logon% %scriptdir%/ssh_command.csh %linux_hostname% %scriptdir% MAKEsmvlinux64.sh

echo.
echo compilation complete
pause
