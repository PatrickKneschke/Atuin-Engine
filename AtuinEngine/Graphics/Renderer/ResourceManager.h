
#pragma once


#include "Mesh.h"
#include "Material.h"
#include "Core/Util/Types.h"
#include "Core/DataStructures/Map.h"

#include <string>


namespace Atuin {


struct ImageData {

    Array<char> pixelData;
    U32 width;
    U32 height;
};


class ResourceManager {

    template<class ResourceType>
    struct Resource {

        ResourceType resource;
        std::string resourcePath;
        U32 useCount;
    };

public:

    MeshData* GetMesh( std::string_view meshPath);
    ImageData* GetImage( std::string_view imagePath);

private:


    void LoadMesh( std::string_view meshPath);
    void LoadImage( std::string_view imagePath);

    Map<U64, Resource<MeshData>> mMeshResources;
    Map<U64, Resource<ImageData>> mImageResources;
};

    
} // namespace Atuin
