
add_library(DataStructures
    Array.cpp 
    Queue.cpp 
    Array.h 
    Queue.h
)

target_link_libraries(DataStructures
    PRIVATE Memory 
)