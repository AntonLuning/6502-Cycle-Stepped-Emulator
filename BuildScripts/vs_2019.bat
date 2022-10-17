@echo off
pushd %~dp0\..\
rmdir /s /q bin
call Vendor\premake\premake5.exe vs2019
popd
PAUSE