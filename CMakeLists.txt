cmake_minimum_required(VERSION 3.6)
project(DitheringEngine LANGUAGES C)

option(BUILD_TESTS "Build tests of libraries" OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

# standards
set(CMAKE_C_STANDARD 99)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)


function(msvc_cflags)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /MP")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /Zi")
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /Oxyi /Ob2 /OPT:REF /fp:fast /MD")
endfunction()

function(gcc_clang_cflags)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden" PARENT_SCOPE)
    set(CMAKE_C_FLAGS_RELEASE "-Ofast -ffast-math -flto=auto -s -lm" PARENT_SCOPE)
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
    src/color_space.h
    src/canvas.h
    src/dither.h

    src/methods/errdiff.h
    src/methods/bayer.h
)

# executables
add_executable(dither
    src/dithermain.c
    src/utils/fileutils.h
    ${ditherlib}
)
add_executable(make-palette
    src/utils/fileutils.h
    src/make-palette.c
    src/external/stb_image_write.h
)
add_executable(subpixeler
    src/subpixeler.c
    src/external/stb_image.h
    src/external/stb_image_write.h
    src/external/stb_image_resize2.h
)

if(BUILD_TESTS)
    add_executable(testoklab src/testoklab.c
        src/color_space.c
        src/color_space.h
    )
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
