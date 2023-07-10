
add_library(Input
    InputModule.cpp 
    InputSignals.cpp 
    InputModule.h
    InputSignals.h 
)

target_link_libraries(Input 
    PRIVATE glfw
    PRIVATE DataStructures 
    PRIVATE Files
    PRIVATE Memory
)