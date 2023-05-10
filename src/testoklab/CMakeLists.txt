include_directories(${DitheringEngine_SOURCE_DIR}/include/dither_library)
include_directories("${PROJECT_BINARY_DIR}/include/dither_library")
add_executable(testoklab testoklab.cpp)
target_link_libraries(testoklab PUBLIC dithering)

