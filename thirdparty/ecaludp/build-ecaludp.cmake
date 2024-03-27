set(ECALUDP_ENABLE_NPCAP        ${ECAL_CORE_NPCAP_SUPPORT})
set(ECALUDP_BUILD_SAMPLES       OFF)
set(ECALUDP_BUILD_TESTS         OFF)
set(ECALUDP_USE_BUILTIN_ASIO    OFF)
set(ECALUDP_USE_BUILTIN_RECYCLE OFF)
set(ECALUDP_USE_BUILTIN_UDPCAP  OFF)
set(ECALUDP_LIBRARY_TYPE        STATIC)

# Add ecaludp library from subdirectory
add_subdirectory(thirdparty/ecaludp/ecaludp EXCLUDE_FROM_ALL)
add_library(ecaludp::ecaludp ALIAS ecaludp)

# move the ecaludp target to a subdirectory in the IDE
set_property(TARGET ecaludp PROPERTY FOLDER lib/ecaludp)

list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/Modules)