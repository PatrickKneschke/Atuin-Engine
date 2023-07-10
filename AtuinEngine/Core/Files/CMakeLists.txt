
add_library(Files
    FileManager.cpp 
    Files.cpp 
    FileManager.h 
    Files.h 
)


target_link_libraries(Files 
    PRIVATE Debug 
    PRIVATE Jobs 
)