cmake_minimum_required(VERSION 3.6)
project(DitheringEngine LANGUAGES C CXX)

option(BUILD_TESTS "Build tests of libraries" OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# standards
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 99)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# standard settings
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)


function(msvc_cflags)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Zi")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Oxyi /Ob2 /OPT:REF /fp:fast /MD")
endfunction()

function(gcc_clang_cflags)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden" PARENT_SCOPE)
    set(CMAKE_C_FLAGS_RELEASE "-O3 -flto=auto -s -lm" PARENT_SCOPE)
    set(CMAKE_C_FLAGS_DEBUG "-g" PARENT_SCOPE)
endfunction()

if("${CMAKE_C_COMPILER_ID}" STREQUAL "Clang" OR "${CMAKE_C_COMPILER_ID}" STREQUAL "AppleClang")
    if ("${CMAKE_C_SIMULATE_ID}" STREQUAL "MSVC")
        msvc_cflags()
    else()
        gcc_clang_cflags()
    endif()
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "GNU" )
    gcc_clang_cflags()
elseif("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC" )
    msvc_cflags()
endif()

set(ditherlib
    src/dither/dither.h
    src/dither/dither.cpp

    src/dither/image_buffer.h
    src/dither/image_buffer.cpp

    src/dither/color_space.cpp
    src/dither/color_space.h

    src/dither/palette.cpp
    src/dither/palette.h

    src/dither/imghandle.cpp
    src/dither/imghandle.h
)

set(filterlib
    src/filter/design_filter_1d.cpp
    src/filter/design_filter_1d.h
    src/filter/fir_1d.h
)

# oh my god
set(pathlib

    src/path/path.cpp
    src/path/path.h

    src/path/path_choices.cpp
    src/path/path_choices.h

    src/path/path_iterator.cpp
    src/path/path_iterator.h

    src/path/recursive_space_filling_curve.cpp
    src/path/recursive_space_filling_curve.h

    src/path/recursive_space_filling_path.cpp
    src/path/recursive_space_filling_path.h

    src/path/sequential_path.cpp
    src/path/sequential_path.h

    src/path/spiral_path.cpp
    src/path/spiral_path.h

    src/path/zigzag_path.cpp
    src/path/zigzag_path.h

    src/path/asano_curve.cpp
    src/path/asano_curve.h

    src/path/float_rectangle.cpp
    src/path/float_rectangle.h

    src/path/hilbert_curve.cpp
    src/path/hilbert_curve.h

    src/path/murray_curve.cpp
    src/path/murray_curve.h
)


# executables
add_executable(dither src/dithermain.cpp ${ditherlib} ${filterlib} ${pathlib})
add_executable(make-palette src/make-palette.cpp
    src/external/stb_image_write.h
)
add_executable(subpixeler src/subpixeler.cpp
    src/external/stb_image.h
    src/external/stb_image_write.h
    src/external/stb_image_resize2.h
)

if(BUILD_TESTS)
    add_executable(filter-test src/filter/filter_test.cpp
        src/filter/design_filter_1d.h
    )
    add_executable(path-iterator-test src/path/path_iterator_test.cpp ${pathlib})
    add_executable(testoklab src/testoklab.cpp
        src/dither/color_space.cpp
        src/dither/color_space.h
    )
    set(tests filter-test path-iterator-test testoklab)
endif()

if(WIN32)
    target_sources(dither PRIVATE res/win32/dither_res.rc)
    target_sources(make-palette PRIVATE res/win32/makepalette_res.rc)
    target_sources(subpixeler PRIVATE ${PROJECT_SOURCE_DIR}/res/win32/subpixeler_res.rc)
endif()

install(TARGETS dither make-palette subpixeler ${tests}
 LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
 ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
 RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
