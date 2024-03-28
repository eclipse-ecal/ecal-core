#!/bin/bash

cd "$(dirname "$0")/../.."

mkdir -p _01_build_default
cd _01_build_default

cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_PROJECT_TOP_LEVEL_INCLUDES="submodule_dependencies.cmake"
make -j

# Run tests if available
make test
