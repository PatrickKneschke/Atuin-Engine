
add_library(DataStructures
    Array.cpp 
    Queue.cpp 
    PriorityQueue.cpp 
    ConcurrentQueue.cpp 
    Array.h 
    Queue.h 
    PriorityQueue.h 
    ConcurrentQueue.h 
)

target_link_libraries(DataStructures
    PRIVATE Math 
    PRIVATE Memory 
)