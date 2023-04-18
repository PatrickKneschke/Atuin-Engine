
#pragma once


#include "Core/Config/CVar.h"
#include "Core/Util/Types.h"
#include "Core/Memory/StackAllocator.h"
#include "Core/Memory/FreeListAllocator.h"

#include <memory>


namespace Atuin {


class MemoryManager {

    
    class FrameAllocator {

        public:

            FrameAllocator(Size size, IAllocator *parent) : 
                stacks {StackAllocator(size/2, parent), StackAllocator(size/2, parent)}, 
                curr {0} 
            {

            }

            StackAllocator& Current() { return stacks[curr]; }
            void Switch() { curr = abs(curr - 1); }

        private:

            StackAllocator stacks[2];
            int curr;
    };

public:

    MemoryManager();
    ~MemoryManager();


private:

    static CVar<Size>* pLsrMemorySize;
    static CVar<Size>* pSceneMemorySize;
    static CVar<Size>* pFrameMemorySize;
    

    std::unique_ptr<StackAllocator>    pMemoryStack;
    std::unique_ptr<StackAllocator>    pLsrMemory;
    std::unique_ptr<FreeListAllocator> pSceneMemory;
    std::unique_ptr<FrameAllocator>    pFrameMemory;
};

    
} //  Atuin
