
#pragma once


#include "Definitions.h"
#include "Core/Util/Types.h"
#include "Core/DataStructures/Array.h"


namespace Atuin {


enum class PassType : U8 {

    SHADOW = 0,
    OPAQUE,
    TRANSPARENT,
    COUNT
};


template<typename T>
class PassData {

public:

    T& operator[] (PassType pass) {

        return data[ (Size)pass ];
    }

    void Clear(T defaultValue) {

        for (Size i = 0; i < (Size)PassType::COUNT; i++)
        {
            data[i] = defaultValue;
        }        
    }

private:

    T data[(Size)PassType::COUNT];    
};


struct IndirectData {

    vk::DrawIndexedIndirectCommand drawIndirectCmd;
    U32 batchIdx;
    U32 objectIdx;
};


struct InstanceData {

    U32 batchIdx;
    U32 objectIdx;
};


// used to draw one instance of a RenderObject at <objectId>
struct RenderBatch {

    U32 objectIdx;
    U64 sortKey;

    bool operator== (const RenderBatch &rhs) {

        return objectIdx == rhs.objectIdx && sortKey == rhs.sortKey;
    }
};


// used to render <count> instances of a RenderObject at <objectId>
struct IndirectBatch {

    U32 objectIdx;
    U32 first;
    U32 count;
};

// used to render multiple IndicectBatch at once
struct MultiBatch {

    U32 first;
    U32 count;
};


struct MeshPass {

    PassType passType;
    
    // indices of render objects that need to be added to the pass
    Array<U32> unbatchedIndices;
    // indices of render objects participating in this render pass
    Array<U32> objectIndices;
    // indices into the pass object array
    Array<U32> deleteObjectIndices;
    Array<U32> reuseObjectIndices;

    // batches of draw command data sorted by material and mesh
    Array<RenderBatch> renderBatches;
    Array<IndirectBatch> indirectBatches;
    Array<MultiBatch> multiBatches;

    // GPU buffer containing final draw indirects after culling
    Buffer drawIndirectBuffer;
    // GPU buffer containing objectIdx for each instance after culling
    Buffer instanceIndexBuffer;
    // GPU buffer to hold indirect batch index and object index for each instance
    Buffer instanceDataBuffer;

    // dirty flags
    bool hasChanged = false;
};

    
} // Atuin


// comparison operators for RenderBatch
bool operator< (const Atuin::RenderBatch &lhs, const Atuin::RenderBatch &rhs);