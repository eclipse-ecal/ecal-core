
# The eCAL core only project

[![License](https://img.shields.io/github/license/continental/ecal.svg?style=flat)](LICENSE.txt)

The **e**nhanced **C**ommunication **A**bstraction **L**ayer (eCAL) is a middleware that enables scalable, high performance interprocess communication on a single computer node or between different nodes in a computer network. 

The goal of the ecal core only project is to create a modular core with clearly defined dependencies based on the configured features. This will enormously reduce the integration effort into other projects and the porting effort to specific hardware.

Visit the eCAL Documentation at 🌐 https://ecal.io for more information.

## Differences to Eclipse-eCAL
  - C/C++ language support only

## How to build

### Clone the repository and its submodules

```bash
git clone https://github.com/eclipse-ecal/ecal-core.git
cd ecal-core
git submodule init
git submodule update
```

### Build it

```bash
mkdir _build
cd _build
cmake .. -DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=submodule_dependencies.cmake
cmake --build .
```

### CMake Options

This section provides documentation for the CMake options used in configuring the eCAL library. These options allow you to customize the build and enable or disable specific features. Please adapt the options according to your project requirements.

#### Available dependencies

##### `ECAL_CORE_HAS_PROTOBUF`
- This option determines whether the Google protobuf library is available or not. If enabled, it includes the compilation of protobuf based samples and unit tests for eCAL.

#### Additional Builds

##### `ECAL_CORE_BUILD_SAMPLES`
- This option controls whether the eCAL samples should be built. If not needed, you can disable this option to reduce build time.

##### `ECAL_CORE_BUILD_TESTS`
- This option determines whether the eCAL Google Tests should be built. If enabled, it includes the compilation of unit tests for eCAL. Disabling this option will skip the build of tests if they are not required in your development environment.
- Requires gtest library.

#### Core Internal Feature Configuration

##### `ECAL_CORE_CONFIGURATION`
- Enabling this option allows eCAL to be configured via an `ecal.yaml` file. This file is used to set various configuration parameters for eCAL, providing flexibility in adjusting its behavior without modifying the source code.
- Requires yaml-cpp library.

##### `ECAL_CORE_COMMAND_LINE`
- Enabling this option includes support for eCAL application command-line interfaces (cmd line). This allows you to interact with eCAL applications through the command line, providing additional runtime configuration options.
- Requires tclap library.

##### `ECAL_CORE_REGISTRATION`
- Enabling this option activates the eCAL registration layer. This layer is responsible for managing and registering different components within the eCAL ecosystem, facilitating communication between them.

##### `ECAL_CORE_MONITORING`
- Enabling this option activates the eCAL monitoring functionality.

##### `ECAL_CORE_PUBLISHER`
- Enabling this option includes the eCAL publisher functionality. This is essential for components that need to publish data to the eCAL communication infrastructure.

##### `ECAL_CORE_SUBSCRIBER`
- Enabling this option includes the eCAL subscriber functionality. This is crucial for components that need to subscribe to and receive data from the eCAL communication infrastructure.

##### `ECAL_CORE_SERVICE`
- Enabling this option includes the eCAL server/client functionality.

##### `ECAL_CORE_TIMEPLUGIN`
- Enabling this option includes the eCAL time plugin functionality. This allows for precise synchronization of time across different components using eCAL.

##### `ECAL_CORE_NPCAP_SUPPORT`
- Enabling this option replaces the standard ASIO UDP receiver by a NPCAP based UDP receiver to increase the performance on Windows platforms.
- Requires udpcap library.

##### `ECAL_CORE_REGISTRATION_SHM`
- Enabling this option activates the eCAL registration layer based on shared memory for high performance local communication scenarios only.

##### `ECAL_CORE_TRANSPORT_UDP`
- Enabling this option includes the eCAL UDP Multicast message transport layer.
- Requires ecaludp library.

##### `ECAL_CORE_TRANSPORT_TCP`
- Enabling this option includes the eCAL TCP message transport layer.
- Requires tcp_pubsub library.

##### `ECAL_CORE_TRANSPORT_SHM`
- Enabling this option includes the eCAL local shared memory message transport layer.

Note: Please adjust these options based on your project's needs, ensuring that the selected features align with your desired functionality and dependencies.
