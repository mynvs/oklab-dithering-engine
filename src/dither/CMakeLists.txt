include_directories(${DitheringEngine_SOURCE_DIR}/include/dither_library ${DitheringEngine_SOURCE_DIR}/include/path)

include_directories("${PROJECT_BINARY_DIR}/include/dither_library")
add_executable(dither dithermain.cpp)

if("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" )
target_link_libraries(dither PUBLIC dithering paths filter pthread)
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" )
target_link_libraries(dither PUBLIC dithering paths filter pthread)
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC" )
target_link_libraries(dither PUBLIC dithering paths filter)
endif()
