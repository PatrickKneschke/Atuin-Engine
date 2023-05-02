
add_library(Files
    FileManager.cpp 
    FileManager.h
)


target_link_libraries(Files 
    PRIVATE Debug 
)