@echo off

pushd %~dp0\..\..

call build\windows\win_set_vars.bat

cmake --build "%BUILD_DIR_COMPLETE%" --parallel --config Release
cmake --build "%BUILD_DIR_SDK%" --parallel --config Debug

popd
