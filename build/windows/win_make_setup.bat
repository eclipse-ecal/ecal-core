@echo off

pushd %~dp0\..\..

call build\windows\win_set_vars.bat

cd /d %WORKSPACE%\%BUILD_DIR_SDK%
cpack -C Debug

cd /d %WORKSPACE%\%BUILD_DIR_COMPLETE%
cpack -C Release

popd
