@echo off

pushd "%~dp0\..\.."

mkdir _01_build_default
cd _01_build_default

cmake .. -A x64 -DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=submodule_dependencies.cmake
cmake --build . --parallel --config Release
ctest -C Release -V

popd
