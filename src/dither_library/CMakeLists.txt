
include_directories(${DitheringEngine_SOURCE_DIR}/include/dither_library)
add_library(dithering color_space.cpp dither.cpp image_buffer.cpp palette.cpp ppm.cpp)
target_compile_options(dithering PRIVATE -ffast-math)



