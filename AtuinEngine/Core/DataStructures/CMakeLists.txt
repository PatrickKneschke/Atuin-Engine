
add_library(DataStructures
    Array.cpp 
    Queue.cpp 
    PriorityQueue.cpp 
    ConcurrentQueue.cpp 
    Map.cpp
    Json.cpp
    Array.h 
    Queue.h 
    PriorityQueue.h 
    ConcurrentQueue.h 
    Map.h 
    Json.h 
)

target_link_libraries(DataStructures
    PRIVATE Math 
    PRIVATE Memory 
)