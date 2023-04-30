
include_directories( . )

# compile options
set(GCC_COMPILE_OPTIONS "-Wall;-Wextra;-Wformat;-Wshadow;-Wconversion;-pthread;-std=c++20")
set(GCC_COMPILE_DEBUG_OPTIONS "${GCC_COMPILE_OPTIONS};-g;-O0")
set(GCC_COMPILE_RELEASE_OPTIONS "${GCC_COMPILE_OPTIONS};-O3")

add_compile_options(
    "$<$<CONFIG:Debug>:${GCC_COMPILE_DEBUG_OPTIONS}>"
    "$<$<CONFIG:Release>:${GCC_COMPILE_RELEASE_OPTIONS}>"
)

add_subdirectory(Core)

add_executable(${PROJECT_NAME}
    main.cpp
    EngineLoop.cpp
    Application.h
    EngineLoop.h
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE Time 
    PRIVATE Files 
    PRIVATE Config 
    PRIVATE Debug 
    PRIVATE Memory 
    PRIVATE DataStructures
)

# copy config file to binary directory
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.ini
    ${CMAKE_CURRENT_BINARY_DIR}/config.ini 
    COPYONLY
)