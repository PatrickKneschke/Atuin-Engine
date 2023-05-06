
add_library(DataStructures
    Array.cpp 
    Queue.cpp 
    PriorityQueue.cpp 
    Array.h 
    Queue.h 
    PriorityQueue.h 
)

target_link_libraries(DataStructures
    PRIVATE Math 
    PRIVATE Memory 
)