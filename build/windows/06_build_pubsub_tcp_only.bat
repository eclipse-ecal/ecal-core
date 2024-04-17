@echo off

pushd "%~dp0\..\.."

set CMAKE_OPTIONS=-DCMAKE_INSTALL_PREFIX=_install ^
-DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=submodule_dependencies.cmake ^
-DECAL_CORE_HAS_PROTOBUF=OFF ^
-DECAL_CORE_BUILD_SAMPLES=ON ^
-DECAL_CORE_BUILD_TESTS=ON ^
-DECAL_CORE_CONFIG_INIFILE=OFF ^
-DECAL_CORE_COMMAND_LINE=OFF ^
-DECAL_CORE_REGISTRATION=OFF ^
-DECAL_CORE_REGISTRATION_SHM=OFF ^
-DECAL_CORE_MONITORING=OFF ^
-DECAL_CORE_PUBLISHER=ON ^
-DECAL_CORE_SUBSCRIBER=ON ^
-DECAL_CORE_SERVICE=OFF ^
-DECAL_CORE_TIMEPLUGIN=OFF ^
-DECAL_CORE_TRANSPORT_UDP=OFF ^
-DECAL_CORE_TRANSPORT_TCP=ON ^
-DECAL_CORE_TRANSPORT_SHM=OFF ^
-DECAL_CORE_NPCAP_SUPPORT=OFF

mkdir _06_build_pubsub_tcp_only
cd _06_build_pubsub_tcp_only

cmake .. -A x64 %CMAKE_OPTIONS%
cmake --build . --parallel --config Release
ctest -C Release -V

popd
