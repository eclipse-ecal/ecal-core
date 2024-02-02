#!/bin/bash

cd "$(dirname "$0")/../.."

mkdir -p _01_build_default
cd _01_build_default

cmake .. -DCMAKE_BUILD_TYPE=Release
make -j

# Run tests if available
make test
