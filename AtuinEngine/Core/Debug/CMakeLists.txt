
add_library(Debug 
    Logger.cpp 
    Definitions.cpp 
    Logger.h 
    Definitions.h
)

target_link_libraries(Debug 
    PRIVATE Time 
    PRIVATE Files
)