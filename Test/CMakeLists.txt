

add_executable(TestAll
    TestMain.cpp 
)

add_subdirectory(Core)

target_link_libraries(TestAll
    PRIVATE DataStructures
)
