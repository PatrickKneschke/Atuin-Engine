
#pragma once


#include "Definitions.h"
#include "Core/Util/Types.h"
#include "Core/DataStructures/Array.h"

#include "vulkan/vulkan.hpp"


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

    void Clear() {

        for (Size i = 0; i < (Size)PassType::COUNT; i++)
        {
            data[i] = T();
        }        
    }

private:

    T data[(Size)PassType::COUNT];    
};


struct RenderObject {

    glm::mat4 transform;
    glm::vec4 sphereBounds;

    U64 meshIdx;
    U64 materialIdx;

    PassData<I64> passIndex;
};

// used to draw one instance of a RenderObject at <objectId>
struct RenderBatch {

    U64 objectIdx;
};

// used to render <count> instances of a RenderObject at <objectId>
struct IndirectBatch {

    U64 objectIdx;
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
    
    // objects participating in this render pass
    Array<RenderObject> renderObjects;
    // indices into the object array
    Array<U64> unbatchedObjects;
    Array<U64> deleteObjects;
    Array<U64> reuseObjects;

    // batches of draw command data sorted by material and mesh
    Array<RenderBatch> renderBatches;
    Array<IndirectBatch> indirectBatches;
    Array<MultiBatch> multiBatches;

    // GPU buffer containing final draw indirects after culling
    Buffer drawIndirectBuffer;
    // GPU buffer containing objectId for each instance after culling
    Buffer instanceObjBuffer;
    // GPU buffer to hold indirect batch ID and object ID for each instance
    Buffer instanceDataBuffer;

    // dirty flags
    bool rebuildBatches;
    bool rebuildInstances;
};

    
} // Atuin
