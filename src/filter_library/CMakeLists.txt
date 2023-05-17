
include_directories(${DitheringEngine_SOURCE_DIR}/external/pocketfftcpp)
add_library(filter design_filter_1d.cpp)
add_executable(filter_test filter_test.cpp)

if("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" )
target_link_libraries(filter_test pthread)
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" )
target_link_libraries(filter_test pthread)
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC" )
target_link_libraries(filter_test)
endif()
