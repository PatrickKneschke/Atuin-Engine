
#include "Memory.h"
#include "MemoryManager.h"


namespace Atuin {
    

MemoryManager* Memory::sMemoryManager =  nullptr;


void* Memory::Allocate(Size size, U8 alignment) {

    if (pMemoryManager != nullptr)
    {
        return pMemoryManager->Allocate(size, alignment);
    }

    return malloc(size);
}


void Memory::Free(void *ptr) {

    if(pMemoryManager != nullptr)
    {
        return pMemoryManager->Free(ptr);
    }

    return free(ptr);

}


} // Atuin
