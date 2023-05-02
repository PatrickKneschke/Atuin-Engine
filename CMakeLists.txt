
cmake_minimum_required(VERSION 3.16)

project(AtuinEngine 
            VERSION     0.1
            LANGUAGES   CXX
)

# compile options
set(GCC_COMPILE_OPTIONS "-Wall;-Wextra;-Wformat;-Wshadow;-Wconversion;-pthread;-std=c++20")
set(GCC_COMPILE_DEBUG_OPTIONS "${GCC_COMPILE_OPTIONS};-g;-O0")
set(GCC_COMPILE_RELEASE_OPTIONS "${GCC_COMPILE_OPTIONS};-O3")

add_compile_options(
    "$<$<CONFIG:Debug>:${GCC_COMPILE_DEBUG_OPTIONS}>"
    "$<$<CONFIG:Release>:${GCC_COMPILE_RELEASE_OPTIONS}>"
)

include_directories( ./AtuinEngine )

add_subdirectory(AtuinEngine)
add_subdirectory(Test)