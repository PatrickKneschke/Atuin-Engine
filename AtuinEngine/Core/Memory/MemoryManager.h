
#pragma once


#include "Core/Config/CVar.h"
#include "Core/Util/Types.h"

#include <memory>


namespace Atuin {


class StackAllocator;
class FreeListAllocator;

class MemoryManager {

    
    struct DoubleStackAllocator {

        StackAllocator *pFront = nullptr;
        StackAllocator *pBack = nullptr;

        void Switch() { 
            
            auto pTemp = pFront;
            pFront = pBack;
            pBack = pTemp;
        }
    };

public:

    MemoryManager();
    ~MemoryManager();


private:

    static CVar<Size>* pLsrMemorySize;
    static CVar<Size>* pSceneMemorySize;
    static CVar<Size>* pFrameMemorySize;
    

    std::unique_ptr<StackAllocator>         pMemoryStack;
    std::unique_ptr<StackAllocator>         pLsrMemory;
    std::unique_ptr<FreeListAllocator>      pSceneMemory;
    std::unique_ptr<DoubleStackAllocator>   pFrameMemory;
};

    
} //  Atuin
