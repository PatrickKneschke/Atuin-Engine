
add_library(Debug 
    Logger.cpp 
    Log.cpp 
    Definitions.cpp 
    Logger.h 
    Log.h 
    Definitions.h
)

target_link_libraries(Debug 
    PRIVATE Time 
    PRIVATE Files
)