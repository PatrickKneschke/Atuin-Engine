
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

    void Clear(T defaultValue) {

        for (Size i = 0; i < (Size)PassType::COUNT; i++)
        {
            data[i] = defaultValue;
        }        
    }

private:

    T data[(Size)PassType::COUNT];    
};


struct RenderObject {

    glm::mat4 transform;
    glm::vec4 sphereBounds;

    U32 meshIdx;
    U32 materialIdx;

    PassData<I64> passIndex;
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
    
    // objects in pass that need to be processed into batches
    Array<RenderObject> unbatchedObjects;
    // objects participating in this render pass
    Array<RenderObject> renderObjects;
    // indices into the object array
    Array<U32> deleteObjects;
    Array<U32> reuseObjects;

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


// comparison operators for RenderBatch
bool operator< (const Atuin::RenderBatch &lhs, const Atuin::RenderBatch &rhs);