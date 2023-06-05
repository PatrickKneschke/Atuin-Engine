
add_library(Input
    InputModule.cpp 
    KeyBinding.cpp 
    InputModule.h
    KeyBinding.h 
)

target_link_libraries(Input 
    PRIVATE glfw
)