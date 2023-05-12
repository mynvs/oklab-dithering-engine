
include_directories(${DitheringEngine_SOURCE_DIR}/external/pocketfftcpp)
add_library(filter design_filter_1d.cpp)
add_executable(filter_test filter_test.cpp)
target_link_libraries(filter_test pthread)


