#!/bin/bash

cd "$(dirname "$0")/../.."

CMAKE_OPTIONS="-DCMAKE_INSTALL_PREFIX=_install \
-DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=submodule_dependencies.cmake \
-DECAL_CORE_HAS_PROTOBUF=ON \
-DECAL_CORE_BUILD_SAMPLES=ON \
-DECAL_CORE_BUILD_TESTS=ON \
-DECAL_CORE_CONFIG_INIFILE=OFF \
-DECAL_CORE_COMMAND_LINE=OFF \
-DECAL_CORE_REGISTRATION=ON \
-DECAL_CORE_REGISTRATION_SHM=ON \
-DECAL_CORE_MONITORING=OFF \
-DECAL_CORE_PUBLISHER=OFF \
-DECAL_CORE_SUBSCRIBER=OFF \
-DECAL_CORE_SERVICE=ON \
-DECAL_CORE_TIMEPLUGIN=OFF \
-DECAL_CORE_TRANSPORT_UDP=OFF \
-DECAL_CORE_TRANSPORT_TCP=OFF \
-DECAL_CORE_TRANSPORT_SHM=OFF \
-DECAL_CORE_NPCAP_SUPPORT=OFF"

mkdir -p _09_build_clientserver_proto
cd _09_build_clientserver_proto

cmake .. -DCMAKE_BUILD_TYPE=Release ${CMAKE_OPTIONS}
make -j

# Run tests if available
make test
