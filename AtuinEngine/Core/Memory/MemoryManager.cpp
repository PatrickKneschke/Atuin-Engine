
#include "MemoryManager.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Memory/StackAllocator.h"
#include "Core/Memory/FreeListAllocator.h"


namespace Atuin {


CVar<Size>* MemoryManager::pLsrMemorySize = ConfigManager::RegisterCVar("Memory", "LSR_MEMORY_SIZE", 32_MB);
CVar<Size>* MemoryManager::pSceneMemorySize = ConfigManager::RegisterCVar("Memory", "SCENE_MEMORY_SIZE", 256_MB);
CVar<Size>* MemoryManager::pFrameMemorySize = ConfigManager::RegisterCVar("Memory", "FRAME_MEMORY_SIZE", 16_MB);


MemoryManager::MemoryManager() {

    pMemoryStack = std::make_unique<StackAllocator>(pLsrMemorySize->Get() + pSceneMemorySize->Get() + pFrameMemorySize->Get() + sizeof(max_align_t));

    pLsrMemory   = std::make_unique<StackAllocator>(pLsrMemorySize->Get(), pMemoryStack.get());
    pSceneMemory = std::make_unique<FreeListAllocator>(pSceneMemorySize->Get(), pMemoryStack.get());
    pFrameMemory = std::make_unique<FrameAllocator>(pFrameMemorySize->Get(), pMemoryStack.get());
}


MemoryManager::~MemoryManager() {

}


} // Atuin