
add_subdirectory(Renderer)

add_library(Graphics
    WindowModule.cpp 
    RenderModule.cpp
    MeshComponent.cpp  
    WindowModule.h
    RenderModule.h 
    MeshComponent.h 
)

target_link_libraries(Graphics 
    PRIVATE glfw
    PRIVATE Renderer 
)