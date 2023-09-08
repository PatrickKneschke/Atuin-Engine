
#include "ResourceManager.h"
#include "Core/Util/StringID.h"


namespace Atuin {


MeshData* ResourceManager::GetMesh( std::string_view meshPath) {

    U64 meshId = SID( meshPath.data());
    if ( mMeshResources.Find( meshId) == mMeshResources.End())
    {
        LoadMesh( meshPath);
    }

    return &mMeshResources.At( meshId).resource;
}


ImageData* ResourceManager::GetImage( std::string_view imagePath) {

    U64 imageId = SID( imagePath.data());
    if ( mImageResources.Find( imageId) == mImageResources.End())
    {
        LoadMesh( imagePath);
    }

    return &mImageResources.At( imageId).resource;
}


void ResourceManager::LoadMesh( std::string_view meshPath) {

    // load mesh data from file
    tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string warning, error;
		
	if( !tinyobj::LoadObj(&attrib, &shapes, &materials, &warning, &error, meshPath.data()) ) 
	{
		throw std::runtime_error(warning + error);
	}

    // insert new  mesh resource
    U64 meshId = SID( meshPath.data());
    Resource<MeshData> &mesh = mMeshResources[ meshId];
    mesh.resourcePath = meshPath;
    auto &vertices = mesh.resource.vertices;
    auto &indices = mesh.resource.indices;
		
    // process vertex and index data
	std::unordered_map<Vertex, uint32_t> uniqueVertices;
	for(const auto &shape : shapes) 
	{
		for(const auto &index : shape.mesh.indices) 
		{
			Vertex vertex{};				
			vertex.position = {
				attrib.vertices[3*index.vertex_index + 0],
				attrib.vertices[3*index.vertex_index + 1],
				attrib.vertices[3*index.vertex_index + 2]
			};
			vertex.texCoord = {
				attrib.texcoords[2*index.texcoord_index + 0],
				1.0f - attrib.texcoords[2*index.texcoord_index + 1]
			};
			vertex.normal = {
				attrib.normals[3*index.normal_index + 0],
				attrib.normals[3*index.normal_index + 1],
				attrib.normals[3*index.normal_index + 2]
			};
			vertex.color = {1.0f, 1.0f, 1.0f, 1.0f};			
				
			if(uniqueVertices.find(vertex) == uniqueVertices.end()) 
			{
				uniqueVertices[vertex] = static_cast<uint32_t>( vertices.GetSize());
				vertices.PushBack(vertex);
			}
				
			indices.PushBack( uniqueVertices[vertex]);
		}
	}

	// calculate tangents
	for (Size i = 0 ; i < indices.GetSize(); i += 3) 
	{
		Vertex& v0 = vertices[ indices[i]];
		Vertex& v1 = vertices[ indices[i+1]];
		Vertex& v2 = vertices[ indices[i+2]];

		glm::vec3 edge1 = v1.position - v0.position;
		glm::vec3 edge2 = v2.position - v0.position;

		float deltaU1 = v1.texCoord.x - v0.texCoord.x;
		float deltaV1 = v1.texCoord.y - v0.texCoord.y;
		float deltaU2 = v2.texCoord.x - v0.texCoord.x;
		float deltaV2 = v2.texCoord.y - v0.texCoord.y;

		float f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);
		glm::vec3 tangent = f * (deltaV2 * edge1 - deltaV1 * edge2);
		
		v0.tangent += tangent;
		v1.tangent += tangent;
		v2.tangent += tangent;
	}
}


void ResourceManager::LoadImage( std::string_view imagePath) {

    // load image data from file
	int width, height, channels;
	stbi_uc *pixels = stbi_load( imagePath.data(), &width, &height, &channels, STBI_rgb_alpha);
	if (!pixels)
	{
		throw std::runtime_error("Failed to load texture image!");
	}
    Size imageSize = width * height * 4;

    U64 imageId = SID( imagePath.data());
    Resource<ImageData> image = mImageResources[ imageId];
    image.resourcePath = imagePath;
    image.resource.width = (U32)width;
    image.resource.height = (U32)height;
    image.resource.pixelData.Resize( imageSize);
    memcpy( image.resource.pixelData.Data(), pixels, imageSize);
}

    
} // namespace Atuin
