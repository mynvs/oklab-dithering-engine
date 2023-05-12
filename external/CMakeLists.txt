include(FetchContent)
FetchContent_Declare(pocketfft
                GIT_REPOSITORY "https://gitlab.mpcdf.mpg.de/mtr/pocketfft.git"
                GIT_TAG "origin/master"
                SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/pocketfft"
)

include(FetchContent)
FetchContent_Declare(pocketfftcpp
                GIT_REPOSITORY "https://gitlab.mpcdf.mpg.de/mtr/pocketfft.git"
                GIT_TAG "origin/cpp"
                SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/pocketfftcpp"
)
FetchContent_MakeAvailable(pocketfft pocketfftcpp)

add_library(pocketfft pocketfft/pocketfft.c)
set_target_properties(pocketfft PROPERTIES LINKER_LANGUAGE C)

include_directories(${CMAKE_CURRENT_LIST_DIR}/pocketfft)
include_directories(${CMAKE_CURRENT_LIST_DIR}/pocketfftcpp)

