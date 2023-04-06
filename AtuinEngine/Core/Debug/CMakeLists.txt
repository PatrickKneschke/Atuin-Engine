
add_library(Debug 
    Logger.cpp 
    Logger.h 
)

target_link_libraries(Debug 
    PRIVATE Files
)