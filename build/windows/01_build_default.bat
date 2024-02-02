@echo off

pushd "%~dp0\..\.."

mkdir _01_build_default
cd _01_build_default

cmake .. -A x64
cmake --build . --parallel --config Release
ctest -C Release -V

popd
