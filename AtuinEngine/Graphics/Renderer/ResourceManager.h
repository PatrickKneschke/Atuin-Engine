
#pragma once


#include "Mesh.h"
#include "Material.h"
#include "Core/Util/Types.h"
#include "Core/DataStructures/Map.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define TINYOBJLOADER_IMPLEMENTATION
#include "tinyobjloader/tiny_obj_loader.h"

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

    /* @brief Requests pointer to an allocated resource of type <ResourceType>. Will load resource from <resourcePath> if it is not cached yet.
     * @param resourcePath    The path to the requested resource  
     * @return    Pointer to allocated resource
     */
    template<class ResourceType>
    ResourceType* Get( std::string_view resourcePath) {

        ResourceType *resource = nullptr;
        switch( typeid(ResourceType))
        {
            case typeid(Mesh) :
                resource = GetMesh( resourcePath);
                break;
            case typeid(Image) :
                resource = GetImage( resourcePath);
                break;

            default:
                break;
        }

        return resource;
    }

    /* @brief Requestsdeletion of an allocated resource of type <ResourceType>. Decrements the resources <useCount> and only deletes if the count reached zero.
     * @param resourcePath    The id of the resource in the cache  
     * @return    <true> if the resource was deleted, <false> if not 
     */
    template<class ResourceType>
    bool Delete( std::string_view resourcePath) {

        U64 id = SID( resourcePath.data());
        switch( typeid(ResourceType))
        {
            case typeid(Mesh) :
                if( --mMeshResources[ id].useCount == 0 )
                {
                    mMeshResources.Erase( id);
                    return true;
                }
            case typeid(Image) :
                if( --mImageResources[ id].useCount == 0 )
                {
                    mImageResources.Erase( id);
                    return true;
                }

            default:
                break;
        }

        return false;
    }


private:

    MeshData* GetMesh( std::string_view meshPath);
    ImageData* GetImage( std::string_view imagePath);

    void LoadMesh( std::string_view meshPath);
    void LoadImage( std::string_view imagePath);

    Map<U64, Resource<MeshData>> mMeshResources;
    Map<U64, Resource<ImageData>> mImageResources;
};

    
} // namespace Atuin
