include_directories(${DitheringEngine_SOURCE_DIR}/include/path)
add_library(paths asano_curve.cpp float_rectangle.cpp murray_curve.cpp  recursive_space_filling_curve.cpp sequential_path.cpp zigzag_path.cpp
                  path_choices.cpp path_iterator.cpp hilbert_curve.cpp path.cpp recursive_space_filling_path.cpp spiral_path.cpp)

add_executable(path_iterator_test path_iterator_test.cpp)
target_link_libraries(path_iterator_test paths)



