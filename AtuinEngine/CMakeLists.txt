
add_subdirectory(Core)

add_executable(${PROJECT_NAME}
    main.cpp
    EngineLoop.cpp
    Application.h
    EngineLoop.h
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE Math 
    PRIVATE Time 
    PRIVATE Files 
    PRIVATE Config 
    PRIVATE Debug 
    PRIVATE Memory 
    PRIVATE DataStructures
    PRIVATE Jobs 
)

# copy config file to binary directory
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/config.ini
    ${CMAKE_CURRENT_BINARY_DIR}/config.ini 
    COPYONLY
)