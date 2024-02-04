# ========================= eCAL LICENSE =================================
#
# Copyright (C) 2016 - 2019 Continental Corporation
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
# ========================= eCAL LICENSE =================================

cmake_minimum_required(VERSION 3.13)

include(CMakeDependentOption)

list(APPEND CMAKE_MODULE_PATH
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)
  
set(eCAL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

if(MSVC)
  set(CMAKE_GENERATOR_PLATFORM x64)
endif()

project(eCAL)
set(ECAL_PROJECT_ROOT "${CMAKE_CURRENT_LIST_DIR}")

# --------------------------------------------------------
# set msvc specific options
# --------------------------------------------------------
if(MSVC)
  message(STATUS "MSVC detected - Adding flags")
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS)
  # support Windows 7 and newer
  add_definitions(-D_WIN32_WINNT=0x0601)
endif()

# --------------------------------------------------------
# set gcc specific options
# --------------------------------------------------------
if(UNIX)
  message(STATUS "GCC detected - Adding flags")
  add_definitions(-Wall -Wextra)

  set(ATOMIC_TEST_CODE "
      #include <atomic>
      int main() { std::atomic<int64_t> i(0); i++; return 0; }
  ")
  include(CheckCXXSourceCompiles)
  check_cxx_source_compiles("${ATOMIC_TEST_CODE}" ATOMIC_IS_BUILTIN)
  if (NOT ATOMIC_IS_BUILTIN)
      message(STATUS "Adding -latomic flag, as this plattform does not support 64bit atomic operations out of the box.")
      set(CMAKE_CXX_LINK_FLAGS "${CMAKE_CXX_LINK_FLAGS} -latomic")
  endif ()
endif()

# ------------------------------------------------------------------------------------------------------------------------------------------------------------------
# configurable build options
# command line usage: cmake .. -DBUILD_SAMPLES=ON -DBUILD_ECAL_TESTS=OFF -DECAL_CORE_CONFIG_INIFILE=OFF
# ------------------------------------------------------------------------------------------------------------------------------------------------------------------

# ------------------------------------------------------------------------------------------------------------------------------------------------------------------
# additional builds (adapt to your needs)
# ------------------------------------------------------------------------------------------------------------------------------------------------------------------
option(ECAL_CORE_BUILD_SAMPLES                           "Build the eCAL samples"                                                                                ON)
option(ECAL_CORE_BUILD_SAMPLES_PROTOBUF                  "Build the eCAL samples using google protobuf message definition"                                      OFF)
option(ECAL_CORE_BUILD_TESTS                             "Build the eCAL google tests"                                                                           ON)
option(ECAL_CORE_BUILD_TESTS_PROTOBUF                    "Build the eCAL google tests using google protobuf message definition"                                 OFF)

# ------------------------------------------------------------------------------------------------------------------------------------------------------------------
# core internal feature configuration (adapt to your needs)
# ------------------------------------------------------------------------------------------------------------------------------------------------------------------
option(ECAL_CORE_CONFIG_INIFILE                          "Enables to configure eCAL via ecal.ini file"                                                           ON)
option(ECAL_CORE_COMMAND_LINE                            "Enables eCAL application cmd line interfaces"                                                          ON)
option(ECAL_CORE_REGISTRATION                            "Enables the eCAL registration layer"                                                                   ON)
option(ECAL_CORE_MONITORING                              "Enables the eCAL monitoring functionality"                                                            OFF)
option(ECAL_CORE_PUBLISHER                               "Enables the eCAL publisher functionality"                                                              ON)
option(ECAL_CORE_SUBSCRIBER                              "Enables the eCAL subscriber functionality"                                                             ON)
option(ECAL_CORE_SERVICE                                 "Enables the eCAL server/client functionality"                                                          ON)
option(ECAL_CORE_TIMEPLUGIN                              "Enables the eCAL time plugin functionality"                                                            ON)
option(ECAL_CORE_NPCAP_SUPPORT                           "Enable the eCAL Npcap Receiver (Win10 performance fix for UDP communication)"                         OFF)

# ------------------------------------------------------------------------------------------------------------------------------------------------------------------
# core registration layer options (change with care)
# ------------------------------------------------------------------------------------------------------------------------------------------------------------------
option(ECAL_CORE_REGISTRATION_SHM                        "Enables the eCAL registration layer over shared memory"                                                ON)

# ------------------------------------------------------------------------------------------------------------------------------------------------------------------
# core transport layer options (change with care)
# ------------------------------------------------------------------------------------------------------------------------------------------------------------------
option(ECAL_CORE_TRANSPORT_UDP                           "Enables the eCAL to transport payload via UDP multicast"                                               ON)
option(ECAL_CORE_TRANSPORT_TCP                           "Enables the eCAL to transport payload via TCP"                                                        OFF)
option(ECAL_CORE_TRANSPORT_SHM                           "Enables the eCAL to transport payload via local shared memory"                                         ON)

# ------------------------------------------------------------------------------------------------------------------------------------------------------------------
# third party library options (do not change)
# ------------------------------------------------------------------------------------------------------------------------------------------------------------------
option(ECAL_THIRDPARTY_BUILD_ASIO                        "Build asio with eCAL"                                                                                  ON)
option(ECAL_THIRDPARTY_BUILD_CMAKEFUNCTIONS              "Build CMakeFunctions with eCAL"                                                                        ON)

cmake_dependent_option(ECAL_THIRDPARTY_BUILD_GTEST       "Build gtest with eCAL"               ON   "ECAL_CORE_BUILD_TESTS"                                     OFF)
cmake_dependent_option(ECAL_THIRDPARTY_BUILD_TCP_PUBSUB  "Build tcp_pubsub library with eCAL"  ON   "ECAL_CORE_TRANSPORT_TCP"                                   OFF)
cmake_dependent_option(ECAL_THIRDPARTY_BUILD_RECYCLE     "Build steinwurf::recylce with eCAL"  ON   "ECAL_CORE_TRANSPORT_TCP"                                   OFF)
cmake_dependent_option(ECAL_THIRDPARTY_BUILD_SIMPLEINI   "Build simpleini with eCAL"           ON   "ECAL_CORE_CONFIG_INIFILE"                                  OFF)
cmake_dependent_option(ECAL_THIRDPARTY_BUILD_TCLAP       "Build tclap library with eCAL"       ON   "ECAL_CORE_COMMAND_LINE"                                    OFF)

if(WIN32)
  cmake_dependent_option(ECAL_THIRDPARTY_BUILD_PROTOBUF  "Build protobuf with eCAL"           OFF   "NOT ECAL_CORE_BUILD_SAMPLES_PROTOBUF; NOT ECAL_CORE_BUILD_TESTS_PROTOBUF"  ON)
else()
  option(ECAL_CORE_NPCAP_SUPPORT                         "Enable the eCAL Npcap Receiver (i.e. the Win10 performance fix)"                                      OFF)
  option(ECAL_THIRDPARTY_BUILD_PROTOBUF                  "Build protobuf with eCAL"                                                                             OFF)
endif()

cmake_dependent_option(ECAL_THIRDPARTY_BUILD_UDPCAP      "Build udpcap library with eCAL"      ON   "ECAL_CORE_NPCAP_SUPPORT"                                   OFF)

if(NOT ECAL_CORE_REGISTRATION)
  if(ECAL_CORE_TRANSPORT_TCP)
    message(STATUS "Switching ECAL_CORE_TRANSPORT_TCP to OFF because registration (ECAL_CORE_REGISTRATION) is not activated")
	set(ECAL_CORE_TRANSPORT_TCP OFF)
  endif()
  if(ECAL_CORE_TRANSPORT_SHM)
    message(STATUS "Switching ECAL_CORE_TRANSPORT_SHM to OFF because registration (ECAL_CORE_REGISTRATION) is not activated")
	set(ECAL_CORE_TRANSPORT_SHM OFF)
  endif()
  if(ECAL_CORE_MONITORING)
    message(STATUS "Switching ECAL_CORE_MONITORING to OFF because registration (ECAL_CORE_REGISTRATION) is not activated")
	set(ECAL_CORE_MONITORING OFF)
  endif()
endif()

set(ECAL_BUILD_VERSION "0.0.0" CACHE STRING "Inject a build version if not building from a git repository")

include(build_location)

# find_project(eCAL calls shall be ignored, eCAL is build as part of the project)
set(as_subproject eCAL)

if(CMAKE_EXPORT_COMPILE_COMMANDS)
  message(STATUS "Compilation database will be created")
else()
  # to make it visible it is explicitly set to OFF
  set(CMAKE_EXPORT_COMPILE_COMMANDS OFF)
endif()

# Normalize backslashes from Windows paths
file(TO_CMAKE_PATH "${CMAKE_MODULE_PATH}" CMAKE_MODULE_PATH)
file(TO_CMAKE_PATH "${CMAKE_PREFIX_PATH}" CMAKE_PREFIX_PATH)
message(STATUS "Module Path: ${CMAKE_MODULE_PATH}")
message(STATUS "Prefix Path: ${CMAKE_PREFIX_PATH}")

if(MSVC)
  message(STATUS "supress thirdparty warnings for windows platform ..")
  set(CMAKE_CXX_FLAGS_OLD "${CMAKE_CXX_FLAGS}")
  if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
    string(REGEX REPLACE "/W[0-4]" "/W0" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W0")
  endif()
endif()

# This is a list of subprojects, that might be build with eCAL
# according to how CMake options are set
set(possible_subprojects
  asio
  CMakeFunctions
  GTest
  Protobuf
  recycle
  simpleini
  tclap
  tcp_pubsub
  udpcap
)

# if a package does need to be build, include the cmake file with build instructions
foreach (dep IN LISTS possible_subprojects)
  string(TOUPPER ${dep} dep_upper)
  string(TOLOWER ${dep} dep_lower)
  if (ECAL_THIRDPARTY_BUILD_${dep_upper})
    include(thirdparty/${dep_lower}/build-${dep_lower}.cmake)
  endif ()
endforeach ()

if(MSVC)
  message(STATUS "reset thirdparty warnings for windows platform ..")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS_OLD}")
endif()

find_package(CMakeFunctions REQUIRED)

git_revision_information(DEFAULT ${ECAL_BUILD_VERSION})
set(eCAL_VERSION_MAJOR  ${GIT_REVISION_MAJOR})
set(eCAL_VERSION_MINOR  ${GIT_REVISION_MINOR})
set(eCAL_VERSION_PATCH  ${GIT_REVISION_PATCH})
set(eCAL_VERSION_STRING ${eCAL_VERSION_MAJOR}.${eCAL_VERSION_MINOR}.${eCAL_VERSION_PATCH})
set(eCAL_VERSION        ${eCAL_VERSION_STRING})

include(helper_functions/ecal_add_functions)
include(helper_functions/ecal_helper_functions)
include(helper_functions/ecal_install_functions)

if(MSVC)
  set(eCAL_PLATFORM_TOOLSET ${CMAKE_VS_PLATFORM_TOOLSET})
endif()

# --------------------------------------------------------
# offer the user the choice of overriding the installation directories
# --------------------------------------------------------
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH
  "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/CMake/eCAL)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")

include(GNUInstallDirs)
set(eCAL_install_app_dir           ${CMAKE_INSTALL_BINDIR})
set(eCAL_install_archive_dir       ${CMAKE_INSTALL_LIBDIR})
set(eCAL_install_archive_dyn_dir   ${CMAKE_INSTALL_LIBDIR})
set(eCAL_install_bin_dir           ${CMAKE_INSTALL_BINDIR})
set(eCAL_install_cmake_dir         ${CMAKE_INSTALL_LIBDIR}/cmake/eCAL)
set(eCAL_install_config_dir        ${CMAKE_INSTALL_SYSCONFDIR}/ecal)
set(eCAL_install_doc_dir           ${CMAKE_INSTALL_DOCDIR})
set(eCAL_install_include_dir       ${CMAKE_INSTALL_INCLUDEDIR})
set(eCAL_install_lib_dir           ${CMAKE_INSTALL_LIBDIR})
set(eCAL_install_samples_dir       ${CMAKE_INSTALL_BINDIR})
set(eCAL_install_samples_src_dir   ${CMAKE_INSTALL_DATADIR}/ecal/samples/)
set(eCAL_install_tests_dir         ${CMAKE_INSTALL_BINDIR})

set(eCAL_config_dir                ${CMAKE_CURRENT_BINARY_DIR}/_generated/)
set(eCAL_config                    ${eCAL_config_dir}/eCALConfig.cmake)
set(eCAL_config_version            ${eCAL_config_dir}/eCALConfigVersion.cmake)

file(MAKE_DIRECTORY ${eCAL_config_dir})

if(WIN32)
  set(eCAL_install_config_dir            cfg)
  set(eCAL_install_doc_dir               doc)
  set(eCAL_install_samples_dir           samples/bin/)
  set(eCAL_install_samples_src_dir       samples/src/)
  set(eCAL_install_tests_dir             tests)
endif()
set(BUILD_DEPLOY_DIRECTORY ${CMAKE_BINARY_DIR}/_deploy)

set(THREADS_PREFER_PTHREAD_FLAG ON)

if(WIN32)
  set(CMAKE_DEBUG_POSTFIX            d)
  set(CMAKE_MINSIZEREL_POSTFIX       minsize)
  set(CMAKE_RELWITHDEBINFO_POSTFIX   reldbg)
endif()

# --------------------------------------------------------
# core
# --------------------------------------------------------
add_subdirectory(src/core)

# --------------------------------------------------------
# core config
# --------------------------------------------------------
add_subdirectory(src/core/cfg)

# --------------------------------------------------------
# services
# --------------------------------------------------------
if(ECAL_CORE_SERVICE)
  add_subdirectory(src/service)
endif()

# --------------------------------------------------------
# time plugins
# --------------------------------------------------------
if(ECAL_CORE_TIMEPLUGIN)
  add_subdirectory(src/time)
endif()

# --------------------------------------------------------
# utils
# --------------------------------------------------------
add_subdirectory(src/utils)

# --------------------------------------------------------
# samples
# --------------------------------------------------------
if(ECAL_CORE_BUILD_SAMPLES)
  add_subdirectory(samples)
endif()

# --------------------------------------------------------
# tests
# --------------------------------------------------------
if(ECAL_CORE_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

# --------------------------------------------------------
# debscripts
# --------------------------------------------------------
#install(EXPORT eCALTargets DESTINATION cmake NAMESPACE eCAL::)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${eCAL_config_version}
  VERSION ${eCAL_VERSION}
  COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
    cmake/${PROJECT_NAME}Config.cmake.in
    ${eCAL_config}
    INSTALL_DESTINATION ${${PROJECT_NAME}_install_cmake_dir}
    PATH_VARS ${PROJECT_NAME}_install_include_dir ${PROJECT_NAME}_install_cmake_dir
)

install(FILES ${eCAL_config} ${eCAL_config_version}
  DESTINATION ${eCAL_install_cmake_dir}
  COMPONENT sdk
)

install(FILES
  cmake/helper_functions/ecal_add_functions.cmake
  cmake/helper_functions/ecal_helper_functions.cmake
  cmake/helper_functions/ecal_install_functions.cmake
  DESTINATION ${${PROJECT_NAME}_install_cmake_dir}/helper_functions
  COMPONENT sdk
)

install(EXPORT eCALCoreTargets
  FILE ${PROJECT_NAME}Targets.cmake
  DESTINATION ${eCAL_install_cmake_dir}
  NAMESPACE eCAL::
  COMPONENT sdk
)

# Make this package available for packing with CPack
include("${CMAKE_CURRENT_LIST_DIR}/cpack/cpack_config.cmake")

# --------------------------------------------------------
# log settings
# --------------------------------------------------------
message(STATUS "--------------------------------------------------------------------------------")
message(STATUS "Build Options:")
message(STATUS "--------------------------------------------------------------------------------")
message(STATUS "CMAKE_EXPORT_COMPILE_COMMANDS                  : ${CMAKE_EXPORT_COMPILE_COMMANDS}")
message(STATUS "--------------------------------------------------------------------------------")
message(STATUS "ECAL_CORE_BUILD_SAMPLES                        : ${ECAL_CORE_BUILD_SAMPLES}")
message(STATUS "ECAL_CORE_BUILD_SAMPLES_PROTOBUF               : ${ECAL_CORE_BUILD_SAMPLES_PROTOBUF}")
message(STATUS "ECAL_CORE_BUILD_TESTS                          : ${ECAL_CORE_BUILD_TESTS}")
message(STATUS "ECAL_CORE_BUILD_TESTS_PROTOBUF                 : ${ECAL_CORE_BUILD_TESTS_PROTOBUF}")
message(STATUS "--------------------------------------------------------------------------------")
message(STATUS "ECAL_CORE_CONFIG_INIFILE                       : ${ECAL_CORE_CONFIG_INIFILE}")
message(STATUS "ECAL_CORE_COMMAND_LINE                         : ${ECAL_CORE_COMMAND_LINE}")
message(STATUS "ECAL_CORE_REGISTRATION                         : ${ECAL_CORE_REGISTRATION}")
message(STATUS "ECAL_CORE_MONITORING                           : ${ECAL_CORE_MONITORING}")
message(STATUS "ECAL_CORE_PUBLISHER                            : ${ECAL_CORE_PUBLISHER}")
message(STATUS "ECAL_CORE_SUBSCRIBER                           : ${ECAL_CORE_SUBSCRIBER}")
message(STATUS "ECAL_CORE_SERVICE                              : ${ECAL_CORE_SERVICE}")
message(STATUS "ECAL_CORE_TIMEPLUGIN                           : ${ECAL_CORE_TIMEPLUGIN}")
message(STATUS "ECAL_CORE_NPCAP_SUPPORT                        : ${ECAL_CORE_NPCAP_SUPPORT}")
message(STATUS "--------------------------------------------------------------------------------")
message(STATUS "ECAL_CORE_REGISTRATION_SHM                     : ${ECAL_CORE_REGISTRATION_SHM}")
message(STATUS "--------------------------------------------------------------------------------")
message(STATUS "ECAL_CORE_TRANSPORT_UDP                        : ${ECAL_CORE_TRANSPORT_UDP}")
message(STATUS "ECAL_CORE_TRANSPORT_TCP                        : ${ECAL_CORE_TRANSPORT_TCP}")
message(STATUS "ECAL_CORE_TRANSPORT_SHM                        : ${ECAL_CORE_TRANSPORT_SHM}")
message(STATUS "--------------------------------------------------------------------------------")
message(STATUS "ECAL_THIRDPARTY_BUILD_ASIO                     : ${ECAL_THIRDPARTY_BUILD_ASIO}")
message(STATUS "ECAL_THIRDPARTY_BUILD_CMAKEFUNCTIONS           : ${ECAL_THIRDPARTY_BUILD_CMAKEFUNCTIONS}")
message(STATUS "ECAL_THIRDPARTY_BUILD_GTEST                    : ${ECAL_THIRDPARTY_BUILD_GTEST}")
message(STATUS "ECAL_THIRDPARTY_BUILD_PROTOBUF                 : ${ECAL_THIRDPARTY_BUILD_PROTOBUF}")
message(STATUS "ECAL_THIRDPARTY_BUILD_SIMPLEINI                : ${ECAL_THIRDPARTY_BUILD_SIMPLEINI}")
message(STATUS "ECAL_THIRDPARTY_BUILD_TCLAP                    : ${ECAL_THIRDPARTY_BUILD_TCLAP}")
message(STATUS "ECAL_THIRDPARTY_BUILD_UDPCAP                   : ${ECAL_THIRDPARTY_BUILD_UDPCAP}")
message(STATUS "--------------------------------------------------------------------------------")
