# ecal-core

This is the new ecal "core-only" repository.

## How to build


### Install requirements

#### Windows

- [CMake](https://cmake.org/download/)
- [Visual Studio](https://visualstudio.microsoft.com/de/downloads/)
- [Inno Setup](https://jrsoftware.org/isdl.php#stable)

#### Ubuntu 20.04 & 22.04

```
sudo apt update
sudo apt-get install libprotobuf-dev libprotoc-dev protobuf-compiler
```


### Check it out

```
git clone https://github.com/eclipse-ecal/ecal-core.git
git submodule init
git submodule update
```

### Build it

```
mkdir _build
cd _build

cmake ..
cmake --build . --parallel --config Release
cpack
```
