
add_executable(atuinEngine 
    main.cpp
    EngineLoop.cpp
    EngineLoop.h
    Application.h
)

# compile options
set(GCC_COMPILE_OPTIONS "-Wall;-Wextra;-Wformat;-Wshadow;-Wconversion;-pthread;-std=c++17")
set(GCC_COMPILE_DEBUG_OPTIONS "${GCC_COMPILE_OPTIONS};-g;-O0")
set(GCC_COMPILE_RELEASE_OPTIONS "${GCC_COMPILE_OPTIONS};-O3")

target_compile_options(atuinEngine
    PUBLIC "$<$<CONFIG:Debug>:${GCC_COMPILE_DEBUG_OPTIONS}>"
    PUBLIC "$<$<CONFIG:Release>:${GCC_COMPILE_RELEASE_OPTIONS}>"
)