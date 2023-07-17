
add_subdirectory(Renderer)

add_library(Graphics
    WindowModule.cpp 
    RenderModule.cpp 
    WindowModule.h
    RenderModule.h 
)

target_link_libraries(Graphics 
    PRIVATE glfw
    PRIVATE Renderer 
)