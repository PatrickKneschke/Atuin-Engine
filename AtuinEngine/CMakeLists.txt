
include_directories( . )

add_subdirectory(Core)

add_executable(${PROJECT_NAME}
    main.cpp
    EngineLoop.cpp
    Application.h
    EngineLoop.h
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE Time
)

# compile options
set(GCC_COMPILE_OPTIONS "-Wall;-Wextra;-Wformat;-Wshadow;-Wconversion;-pthread;-std=c++17")
set(GCC_COMPILE_DEBUG_OPTIONS "${GCC_COMPILE_OPTIONS};-g;-O0")
set(GCC_COMPILE_RELEASE_OPTIONS "${GCC_COMPILE_OPTIONS};-O3")

target_compile_options(${PROJECT_NAME}
    PRIVATE "$<$<CONFIG:Debug>:${GCC_COMPILE_DEBUG_OPTIONS}>"
    PRIVATE "$<$<CONFIG:Release>:${GCC_COMPILE_RELEASE_OPTIONS}>"
)