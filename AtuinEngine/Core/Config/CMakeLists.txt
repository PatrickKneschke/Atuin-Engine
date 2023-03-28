
add_library(Config
    ConfigManager.cpp
    ConfigManager.h 
    ICVar.h 
    CVar.h 
)

target_link_libraries(Config
    PRIVATE Files 
)