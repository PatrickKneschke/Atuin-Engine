
add_library(Memory 
    MemoryManager.cpp 
    IAllocator.cpp 
    StackAllocator.cpp
    PoolAllocator.cpp 
    FreeListAllocator.cpp 
    FreeTreeAllocator.cpp 
    MemoryManager.h
    IAllocator.h 
    StackAllocator.h 
    PoolAllocator.h 
    FreeListAllocator.h
    FreeTreeAllocator.h 
)


target_link_libraries(Memory 
    PRIVATE Math 
    PRIVATE Config 
    PRIVATE Debug
)