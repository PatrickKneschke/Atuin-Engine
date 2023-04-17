
#include "MemoryManager.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Memory/StackAllocator.h"
#include "Core/Memory/FreeListAllocator.h"


namespace Atuin {


CVar<Size>* MemoryManager::pLsrMemorySize = ConfigManager::RegisterCVar("Memory", "LSR_MEMORY_SIZE", 32_MB);
CVar<Size>* pSceneMemorySize = ConfigManager::RegisterCVar("Memory", "SCENE_MEMORY_SIZE", 256_MB);
CVar<Size>* pFrameMemorySize = ConfigManager::RegisterCVar("Memory", "FRAME_MEMORY_SIZE", 16_MB);


MemoryManager::MemoryManager() {

    pMemoryStack = std::make_unique<StackAllocator>(pLsrMemorySize->Get() + pSceneMemorySize->Get() + 2*pFrameMemorySize->Get());

    pLsrMemory = std::make_unique<StackAllocator>(pLsrMemorySize->Get(), pMemoryStack);
    pSceneMemory = std::make_unique<FreeListAllocator>(pSceneMemorySize->Get(), pMemoryStack);
    pFrameMemory = std::make_unique<DoubleStackAllocator> ();
        pFrameMemory->pFront = new StackAllocator(pFrameMemorySize->Get(), pMemoryStack.get());
        pFrameMemory->pBack = new StackAllocator(pFrameMemorySize->Get(), pMemoryStack.get());
}


MemoryManager::~MemoryManager() {


}


} // Atuin