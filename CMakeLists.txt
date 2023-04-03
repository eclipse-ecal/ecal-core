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

project(eCAL)
set(ECAL_PROJECT_ROOT "${CMAKE_CURRENT_LIST_DIR}")

# --------------------------------------------------------
# set msvc specific options
# --------------------------------------------------------
if(MSVC)
  message(STATUS "MSVC detected - Adding flags")
  set_property(GLOBAL PROPERTY USE_FOLDERS ON)
  add_definitions(-D_SCL_SECURE_NO_WARNINGS -D_CRT_SECURE_NO_WARNINGS)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /W4")
  # support Windows 7 and newer
  add_definitions(-D_WIN32_WINNT=0x0601)
endif()

# --------------------------------------------------------
# set mingw specific options
# --------------------------------------------------------
if(MINGW)
  add_definitions(-std=c++14 -DWINVER=0x0601)
endif()

# --------------------------------------------------------
# set gcc specific options
# --------------------------------------------------------
if(UNIX)
  message(STATUS "GCC detected - Adding flags")
  set(CMAKE_CXX_STANDARD 14)
  add_definitions(-Wall -Wextra -std=c++14)

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

# --------------------------------------------------------
# command line build options
# use it that way cmake .. -DBUILD_SAMPLES=ON
# --------------------------------------------------------
option(HAS_CAPNPROTO                           "Platform supports Cap'n Proto library"                            OFF)

option(BUILD_SAMPLES                           "Build the eCAL samples"                                            ON)
option(BUILD_ECAL_TESTS                        "Build the eCAL google tests"                                       ON)

option(ECAL_JOIN_MULTICAST_TWICE               "Specific Multicast Network Bug Workaround"                        OFF)
option(ECAL_NPCAP_SUPPORT                      "Enable the eCAL Npcap Receiver (i.e. the Win10 performance fix)"  OFF)
option(ECAL_USE_CLOCKLOCK_MUTEX                "Use native mutex with monotonic clock (requires glibc >= 2.30)"   OFF)

option(ECAL_LAYER_ICEORYX                      "Use iceoryx shared memory as local communication layer"           OFF)

option(ECAL_INSTALL_SAMPLE_SOURCES             "Install the sources of eCAL samples"                               ON)

# Set option regarding third party library builds
option(ECAL_THIRDPARTY_BUILD_CMAKE_FUNCTIONS   "Build CMakeFunctions with eCAL"                                    ON)
option(ECAL_THIRDPARTY_BUILD_TCP_PUBSUB        "Build tcp_pubsub library with eCAL"                                ON)
option(ECAL_THIRDPARTY_BUILD_RECYCLE           "Build steinwurf::recylce with eCAL"                                ON)
option(ECAL_THIRDPARTY_BUILD_GTEST             "Build gtest with eCAL"                                             ON)
option(ECAL_THIRDPARTY_BUILD_UDPCAP            "Build udpcap library with eCAL"                                   OFF)
if(WIN32)
  option(ECAL_THIRDPARTY_BUILD_PROTOBUF        "Build protobuf with eCAL"                                          ON)
else()
  option(ECAL_THIRDPARTY_BUILD_PROTOBUF        "Build protobuf with eCAL"                                         OFF)
endif()

option(CPACK_PACK_WITH_INNOSETUP               "Create Innosetup installer for the Windows build"                  ON)

set(ECAL_BUILD_VERSION "0.0.0" CACHE STRING "Inject a build version if not building from a git repository")

# there is a CMake issue with testing threading availibility via TEST_RUN
if(${CMAKE_CROSSCOMPILING})
  set(THREADS_PTHREAD_ARG "2" CACHE STRING "Forcibly set by CMakeLists.txt." FORCE)
endif()

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
# according to how options ECAL_BUILD_<SUBPROJECT> are set
set(possible_subprojects
  Protobuf
  recycle
  tcp_pubsub
  udpcap
  GTest
  CMakeFunctions
)

# We should rename the option, but don't know how to do in in a
# backwards compatible way
set(ECAL_THIRDPARTY_BUILD_CMAKEFUNCTIONS ${ECAL_THIRDPARTY_BUILD_CMAKE_FUNCTIONS})

# For each dependency, check if option to build was set or not
# if so append to `as_subproject` list
foreach (dep IN LISTS possible_subprojects)
  string(TOUPPER ${dep} dep_upper)
  if (ECAL_THIRDPARTY_BUILD_${dep_upper})
    list(APPEND as_subproject ${dep})
  endif ()
endforeach()

macro(find_package)
  if(NOT "${ARGV0}" IN_LIST as_subproject)
    _find_package(${ARGV})
  endif()
endmacro()

# if a package does need to be build, include the cmake file with build instructions
foreach (dep IN LISTS possible_subprojects)
  string(TOUPPER ${dep} dep_upper)
  string(TOLOWER ${dep} dep_lower)
  if (ECAL_THIRDPARTY_BUILD_${dep_upper})
    include(thirdparty/build-${dep_lower}.cmake)
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

set(eCAL_config_dir                ${eCAL_BINARY_DIR}/cmake/)
set(eCAL_config                    ${eCAL_config_dir}/eCALConfig.cmake)
set(eCAL_config_version            ${eCAL_config_dir}/eCALConfigVersion.cmake)
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
# ecal core internal protobuf
# --------------------------------------------------------
add_subdirectory(ecal/core_pb)

# --------------------------------------------------------
# ecal core
# --------------------------------------------------------
add_subdirectory(ecal/core)

# --------------------------------------------------------
# custom libs
# --------------------------------------------------------
add_subdirectory(lib/ecal_protobuf)
add_subdirectory(thirdparty/ecal-utils)

# --------------------------------------------------------
# samples
# --------------------------------------------------------
if(BUILD_SAMPLES)
  add_subdirectory(samples)
endif()

# ------------------------------------------------------
# unit tests
# --------------------------------------------------------
if(BUILD_ECAL_TESTS)
  enable_testing()

  #add_subdirectory(testing/ecal/clientserver_test)
  add_subdirectory(testing/ecal/core_test)
  add_subdirectory(testing/ecal/event_test)
  add_subdirectory(testing/ecal/expmap_test)
  add_subdirectory(testing/ecal/io_memfile_test)
  add_subdirectory(testing/ecal/pubsub_inproc_test)
  add_subdirectory(testing/ecal/pubsub_proto_test)
  add_subdirectory(testing/ecal/pubsub_test)
  add_subdirectory(testing/lib/ecal_protobuf/dynproto_test)
  add_subdirectory(testing/lib/ecal_protobuf/ecal_proto_test)
endif()

# --------------------------------------------------------
# configs
# --------------------------------------------------------
add_subdirectory(ecal/core/cfg)

# --------------------------------------------------------
# debscripts
# --------------------------------------------------------
if (UNIX)
  add_subdirectory(cpack/debscripts)
endif()

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

if(ECAL_INSTALL_SAMPLE_SOURCES)
  install(DIRECTORY samples/
    DESTINATION ${eCAL_install_samples_src_dir}
  )
endif()

# --------------------------------------------------------
# create package
# --------------------------------------------------------
include(cpack/cpack_variables.cmake)

message(STATUS "Build Options:")
message(STATUS "--------------------------------------------------------------------------------")
message(STATUS "CMAKE_EXPORT_COMPILE_COMMANDS                  : ${CMAKE_EXPORT_COMPILE_COMMANDS}")
message(STATUS "HAS_CAPNPROTO                                  : ${HAS_CAPNPROTO}")
message(STATUS "BUILD_SAMPLES                                  : ${BUILD_SAMPLES}")
message(STATUS "BUILD_ECAL_TESTS                               : ${BUILD_ECAL_TESTS}")
message(STATUS "ECAL_LAYER_ICEORYX                             : ${ECAL_LAYER_ICEORYX}")
message(STATUS "ECAL_INSTALL_SAMPLE_SOURCES                    : ${ECAL_INSTALL_SAMPLE_SOURCES}")
message(STATUS "ECAL_JOIN_MULTICAST_TWICE                      : ${ECAL_JOIN_MULTICAST_TWICE}")
message(STATUS "ECAL_NPCAP_SUPPORT                             : ${ECAL_NPCAP_SUPPORT}")
message(STATUS "ECAL_THIRDPARTY_BUILD_CMAKE_FUNCTIONS          : ${ECAL_THIRDPARTY_BUILD_CMAKE_FUNCTIONS}")
message(STATUS "ECAL_THIRDPARTY_BUILD_TCP_PUBSUB               : ${ECAL_THIRDPARTY_BUILD_TCP_PUBSUB}")
message(STATUS "ECAL_THIRDPARTY_BUILD_RECYCLE                  : ${ECAL_THIRDPARTY_BUILD_RECYCLE}")
message(STATUS "ECAL_THIRDPARTY_BUILD_GTEST                    : ${ECAL_THIRDPARTY_BUILD_GTEST}")
message(STATUS "ECAL_THIRDPARTY_BUILD_PROTOBUF                 : ${ECAL_THIRDPARTY_BUILD_PROTOBUF}")
message(STATUS "ECAL_THIRDPARTY_BUILD_UDPCAP                   : ${ECAL_THIRDPARTY_BUILD_UDPCAP}")
message(STATUS "CPACK_PACK_WITH_INNOSETUP                      : ${CPACK_PACK_WITH_INNOSETUP}")

message(STATUS "--------------------------------------------------------------------------------")
