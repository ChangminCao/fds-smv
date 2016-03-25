@echo off
Title Building 64 bit OSX background

Rem  Windows batch file to build an OSX version of background

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
echo Using the environment variables:
echo.
echo Using GIT revision %smv_revision% to build an OSX version of background

%svn_drive%
cd %svn_root%\smv\scripts
set version=%smv_version%_%smv_revision%

set scriptdir=%linux_svn_root%/SMV/scripts

echo plink %osx_logon% %scriptdir%/ssh_command.sh %osx_hostname% %scriptdir% MAKEbgosx.sh %linux_svn_root%

plink %osx_logon% %scriptdir%/ssh_command.sh %osx_hostname% %scriptdir% MAKEbgosx.sh %linux_svn_root%

echo.
echo compilation complete
pause
