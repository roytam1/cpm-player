echo off
if exist "%ProgramFiles(x86)%" goto is_x64
set path="%ProgramFiles%\Microsoft Visual Studio 9.0\Common7\IDE";%PATH%
goto start
:is_x64
set path="%ProgramFiles(x86)%\Microsoft Visual Studio 9.0\Common7\IDE";%PATH%
:start

devenv.com cpm.vcproj /Rebuild "Release|Win32"

pause
echo on
