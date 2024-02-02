
# The eCAL Cop (core only project)

[![License](https://img.shields.io/github/license/continental/ecal.svg?style=flat)](LICENSE.txt)

<img src="https://github.com/ecal-io/ecal-core-udp/assets/49162693/7f4ab45a-989e-41b6-a44a-b23abfd3d47d" width="400">

The **e**nhanced **C**ommunication **A**bstraction **L**ayer (eCAL) is a middleware that enables scalable, high performance interprocess communication on a single computer node or between different nodes in a computer network. 

This eCAL core version has an reduced functionality compared to [Eclipse-eCAL](https://github.com/eclipse-ecal/ecal) (see _Differences to Eclipse-eCAL_). The goal of this reduced approach is to create a modular core with clearly defined dependencies depending on the configured features.

Visit the eCAL Documentation at 🌐 https://ecal.io for more information.

## Differences to Eclipse-eCAL
  - communication core only, no additional eco system tools
  - C++ and C language support only
  - reduced API functionality (removal of all deprecated functions)

## How to build

### Clone the repository and its submodules

```bash
git clone https://github.com/ecal-io/ecal-cop.git
cd ecal-cop
git submodule init
git submodule update
```

### Build it

```bash
mkdir _build
cd _build
cmake .. -A x64
cmake --build .
```

### CMake Options

This section provides documentation for the CMake options used in configuring the eCAL library. These options allow you to customize the build and enable or disable specific features. Please adapt the options according to your project requirements.

#### Additional Builds

##### `ECAL_CORE_BUILD_SAMPLES` (Default: ON)
- This option controls whether the eCAL samples should be built. If not needed, you can disable this option to reduce build time.

##### `ECAL_CORE_BUILD_SAMPLES_PROTOBUF` (Default: OFF)
- This option controls whether the eCAL protobuf samples should be built. If not needed, you can disable this option to reduce build time.
- Requires google::protobuf serialization library.

##### `ECAL_CORE_BUILD_TESTS` (Default: ON)
- This option determines whether the eCAL Google Tests should be built. If enabled, it includes the compilation of unit tests for eCAL. Disabling this option will skip the build of tests if they are not required in your development environment.
- Requires gtest library.

##### `ECAL_CORE_BUILD_TESTS_PROTOBUF` (Default: OFF)
- This option determines whether the eCAL Google Tests using protobuf serialization should be built.
- Requires gtest library and google::protobuf serialization library.

#### Core Internal Feature Configuration

##### `ECAL_CORE_CONFIG_INIFILE` (Default: ON)
- Enabling this option allows eCAL to be configured via an `ecal.ini` file. This file is used to set various configuration parameters for eCAL, providing flexibility in adjusting its behavior without modifying the source code.
- Requires simpleini library.

##### `ECAL_CORE_COMMAND_LINE` (Default: ON)
- Enabling this option includes support for eCAL application command-line interfaces (cmd line). This allows you to interact with eCAL applications through the command line, providing additional runtime configuration options.
- Requires tclap library.

##### `ECAL_CORE_REGISTRATION` (Default: ON)
- Enabling this option activates the eCAL registration layer. This layer is responsible for managing and registering different components within the eCAL ecosystem, facilitating communication between them.

##### `ECAL_CORE_MONITORING` (Default: OFF)
- Enabling this option activates the eCAL monitoring functionality.

##### `ECAL_CORE_PUBLISHER` (Default: ON)
- Enabling this option includes the eCAL publisher functionality. This is essential for components that need to publish data to the eCAL communication infrastructure.

##### `ECAL_CORE_SUBSCRIBER` (Default: ON)
- Enabling this option includes the eCAL subscriber functionality. This is crucial for components that need to subscribe to and receive data from the eCAL communication infrastructure.

##### `ECAL_CORE_SERVICE` (Default: ON)
- Enabling this option includes the eCAL server/client functionality.

##### `ECAL_CORE_TIMEPLUGIN` (Default: ON)
- Enabling this option includes the eCAL time plugin functionality. This allows for precise synchronization of time across different components using eCAL.

##### `ECAL_CORE_NPCAP_SUPPORT` (Default: OFF)
- Enabling this option replaces the standard ASIO UDP receiver by a NPCAP based UDP receiver to increase the performance on Windows platforms.
- Requires udpcap library.

##### `ECAL_CORE_REGISTRATION_SHM` (Default: ON)
- Enabling this option activates the eCAL registration layer based on shared memory for high performance local communication scenarios only.

##### `ECAL_CORE_TRANSPORT_UDP` (Default: ON)
- Enabling this option includes the eCAL UDP Multicast message transport layer.

##### `ECAL_CORE_TRANSPORT_TCP` (Default: OFF)
- Enabling this option includes the eCAL TCP message transport layer.
- Requires tcp_pubsub library.

##### `ECAL_CORE_TRANSPORT_SHM` (Default: ON)
- Enabling this option includes the eCAL local shared memory message transport layer.

Note: Please adjust these options based on your project's needs, ensuring that the selected features align with your desired functionality and dependencies.
