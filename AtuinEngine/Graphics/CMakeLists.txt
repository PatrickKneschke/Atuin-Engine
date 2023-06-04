
add_library(Graphics
    WindowModule.cpp 
    WindowModule.h
)

target_link_libraries(Graphics 
    PRIVATE glfw
)