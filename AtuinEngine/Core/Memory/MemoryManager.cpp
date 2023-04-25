
#include "MemoryManager.h"
#include "EngineLoop.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Debug/Logger.h"


namespace Atuin {


CVar<Size>* MemoryManager::pHeapMemorySize = ConfigManager::RegisterCVar("Memory", "HEAP_MEMORY_SIZE", 512_MB);


MemoryManager::MemoryManager(EngineLoop *engine) : pEngine {engine} {

    pHeapMemory = std::make_unique<FreeListAllocator>(pHeapMemorySize->Get());
}


void* MemoryManager::Allocate(Size size, U8 alignment) {

    void* mem = nullptr;
    try
    {
        mem = pHeapMemory->Allocate(size, alignment);
    }
    catch(const std::exception& e)
    {
        pEngine->Log()->Error(LogChannel::MEMORY, e.what());
    }
    
    return mem;
}


void  MemoryManager::Free(void *ptr) {

    pHeapMemory->Free(ptr);
}


} // Atuin