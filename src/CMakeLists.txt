if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

set(CMAKE_CXX_FLAGS "-pedantic -Wall -march=native -mtune=native")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

add_compile_options(-pedantic)
add_subdirectory(dither)
add_subdirectory(dither_library)
add_subdirectory(make-palette)
add_subdirectory(subpixeler)
add_subdirectory(testoklab)
set(commands dither make-palette subpixeler testoklab)

install(TARGETS ${commands} DESTINATION bin)
