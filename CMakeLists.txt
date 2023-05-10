cmake_minimum_required(VERSION 3.10)
project(DitheringEngine VERSION 0.1 LANGUAGES CXX)
add_subdirectory(include)
add_subdirectory(src)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)


#mkdir bin && cd src && g++ -pedantic -Wall -ffast-math -I../include color_space.cpp dither.cpp dithermain.cpp image_buffer.cpp palette.cpp ppm.cpp -o ../bin/dither -O3 && cd .. && bin/dither

# TODO 2: Create a project named Tutorial


# TODO 7: Set the project version number as 1.0 in the above project command

# TODO 6: Set the variable CMAKE_CXX_STANDARD to 11
#         and the variable CMAKE_CXX_STANDARD_REQUIRED to True

# TODO 8: Use configure_file to configure and copy TutorialConfig.h.in to
#         TutorialConfig.h

# TODO 3: Add an executable called Tutorial to the project
# Hint: Be sure to specify the source file as tutorial.cxx

# TODO 9: Use target_include_directories to include ${PROJECT_BINARY_DIR}
