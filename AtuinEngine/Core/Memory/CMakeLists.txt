
add_library(Memory 
    MemoryManager.cpp 
    IAllocator.cpp 
    StackAllocator.cpp
    PoolAllocator.cpp 
    FreeListAllocator.cpp    
    MemoryManager.h
    IAllocator.h 
    StackAllocator.h 
    PoolAllocator.h 
    FreeListAllocator.h 
)