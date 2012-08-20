@echo off

Rem Batch file used to update FDS source revision number

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

Rem location of batch files used to set up Intel compilation environment

call %envfile%

%svn_drive%
set win_dir=%svn_root%\SMV\source\
cd %win_dir%

set scriptdir=%linux_svn_root%/SMV/scripts/
set smv_sourcedir=%linux_svn_root%/SMV/source

echo.
echo Updating the directory %smv_sourcedir% in the Linux repository to the SVN revision: %smv_revision%
plink %svn_logon% %scriptdir%/UPDATE_smv_onhost.sh  %smv_sourcedir% %smv_revision% %linux_hostname%

echo.
echo Updating the directory %linux_smvdir% in the the OSX repository on %osx_hostname% to the SVN revision: %smv_revision%
plink %svn_logon% %scriptdir%/UPDATE_smv_onhost.sh  %smv_sourcedir% %smv_revision% %osx_hostname%

pause
