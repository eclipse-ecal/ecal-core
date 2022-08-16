@echo off

pushd %~dp0\..\..

call build\windows\win_make_cmake.bat
call build\windows\win_make_build.bat
call build\windows\win_run_tests.bat
call build\windows\win_make_setup.bat

popd
