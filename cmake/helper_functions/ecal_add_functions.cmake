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

# This function will set the output names of the target according to eCAL conventions.
function(ecal_add_app_console TARGET_NAME)
  add_executable(${TARGET_NAME} ${ARGN})
  ecal_set_subsystem_console(${TARGET_NAME})
  set_target_properties(${TARGET_NAME} PROPERTIES
    VERSION ${eCAL_VERSION_STRING}
    SOVERSION ${eCAL_VERSION_MAJOR}
    OUTPUT_NAME ecal_${TARGET_NAME})
endfunction()

# This helper function automatically adds a gtest to ecal.
# It automatically enables testing and links to the gtest libraries.
function(ecal_add_gtest TARGET_NAME)
  add_executable(${TARGET_NAME} ${ARGN})
  add_test(
    NAME              ${TARGET_NAME} 
    COMMAND           $<TARGET_FILE:${TARGET_NAME}>
    WORKING_DIRECTORY $<TARGET_FILE_DIR:${TARGET_NAME}>
  )
  target_link_libraries(${TARGET_NAME} 
  PRIVATE 
    # Targets from GTestConfig.cmake, FindGtest.cmake ( CMake >= 3.20 )
    $<$<TARGET_EXISTS:GTest::gtest>:GTest::gtest>
    $<$<TARGET_EXISTS:GTest::gtest_main>:GTest::gtest_main>
    # Deprecated Targets from CMake < 3.20, to be removed in the future
    $<$<TARGET_EXISTS:GTest::GTest>:GTest::GTest>
    $<$<TARGET_EXISTS:GTest::Main>:GTest::Main>
  )
  
  #ecal_set_subsystem_windows(${TARGET_NAME})
  set_target_properties(${TARGET_NAME} PROPERTIES
    VERSION ${eCAL_VERSION_STRING}
    SOVERSION ${eCAL_VERSION_MAJOR}
    OUTPUT_NAME ecal_${TARGET_NAME})
endfunction()

function(ecal_add_time_plugin TARGET_NAME)
  add_library(${TARGET_NAME} MODULE ${ARGN})
  set_target_properties(${TARGET_NAME} PROPERTIES
    VERSION ${eCAL_VERSION_STRING}
    SOVERSION ${eCAL_VERSION_MAJOR}
  )
endfunction()

# this appends the 64 / 32 suffix (required for the eCAL Core libraries)
function(ecal_add_ecal_shared_library TARGET_NAME)
  add_library(${TARGET_NAME} SHARED ${ARGN})
  set_target_properties(${TARGET_NAME} PROPERTIES
    VERSION ${eCAL_VERSION_STRING}
    SOVERSION ${eCAL_VERSION_MAJOR}
    OUTPUT_NAME ecal_${TARGET_NAME})
endfunction()

# this appends the 64 / 32 suffix (required for the eCAL Core libraries)
function(ecal_add_ecal_static_library TARGET_NAME)
  add_library(${TARGET_NAME} STATIC ${ARGN})
  set_target_properties(${TARGET_NAME} PROPERTIES
    VERSION ${eCAL_VERSION_STRING}
    SOVERSION ${eCAL_VERSION_MAJOR}
    OUTPUT_NAME ecal_${TARGET_NAME}
    POSITION_INDEPENDENT_CODE ON)
endfunction()

# this appends the 64 / 32 suffix (required for the eCAL Core libraries)
function(ecal_add_ecal_library TARGET_NAME)
  if(BUILD_SHARED_LIBS)
    ecal_add_ecal_shared_library(${TARGET_NAME} ${ARGN})
  else()
    ecal_add_ecal_static_library(${TARGET_NAME} ${ARGN})
  endif()
endfunction()

function(ecal_add_shared_library TARGET_NAME)
  add_library(${TARGET_NAME} SHARED ${ARGN})
  set_target_properties(${TARGET_NAME} PROPERTIES
    VERSION ${eCAL_VERSION_STRING}
    SOVERSION ${eCAL_VERSION_MAJOR}
    OUTPUT_NAME ecal_${TARGET_NAME})
endfunction()

function(ecal_add_static_library TARGET_NAME)
  add_library(${TARGET_NAME} STATIC ${ARGN})
  set_property(TARGET ${TARGET_NAME} PROPERTY POSITION_INDEPENDENT_CODE ON)
  set_target_properties(${TARGET_NAME} PROPERTIES 
    VERSION ${eCAL_VERSION_STRING}
    SOVERSION ${eCAL_VERSION_MAJOR}
    OUTPUT_NAME ecal_${TARGET_NAME})
endfunction()

function(ecal_add_interface_library TARGET_NAME)
  add_library(${TARGET_NAME} INTERFACE)
endfunction()

function(ecal_add_library TARGET_NAME)
if(BUILD_SHARED_LIBS)
  ecal_add_shared_library(${TARGET_NAME} ${ARGN})
else()
  ecal_add_static_library(${TARGET_NAME} ${ARGN})
endif()
endfunction()

function(ecal_add_sample TARGET_NAME)
  add_executable(${TARGET_NAME} ${ARGN})
  set_target_properties(${TARGET_NAME} PROPERTIES
    VERSION ${eCAL_VERSION_STRING}
    SOVERSION ${eCAL_VERSION_MAJOR}
    OUTPUT_NAME ecal_sample_${TARGET_NAME})
endfunction()
