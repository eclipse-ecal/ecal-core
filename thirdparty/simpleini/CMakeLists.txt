add_library(simpleini INTERFACE)
target_include_directories(simpleini INTERFACE ${CMAKE_CURRENT_LIST_DIR}/simpleini)
add_library(simpleini::simpleini ALIAS simpleini)