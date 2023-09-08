
#pragma once


#include "Definitions.h"
#include "Core/Util/Types.h"
#include "Core/DataStructures/Array.h"


namespace Atuin {


// stores all data of a single mesh vertex
struct Vertex {

	glm::vec3 position;
	glm::vec4 color;
	glm::vec2 texCoord;
	glm::vec3 normal;
	glm::vec3 tangent;

	static vk::VertexInputBindingDescription getBindingDescription() {
	
		auto bindingDescription = vk::VertexInputBindingDescription{}
			.setBinding( 0 )
			.setStride( sizeof(Vertex) )
			.setInputRate( vk::VertexInputRate::eVertex );
		
		return bindingDescription;
	}
	
	static Array<vk::VertexInputAttributeDescription> getAttributeDescriptions() {

		Array<vk::VertexInputAttributeDescription> attributeDescriptions;
		attributeDescriptions.Resize(5);
		
		attributeDescriptions[0]
			.setLocation( 0 )
			.setBinding( 0 )
			.setFormat( vk::Format::eR32G32B32Sfloat )
			.setOffset( offsetof(Vertex, position) );
		attributeDescriptions[1]
			.setLocation( 1 )
			.setBinding( 0 )
			.setFormat( vk::Format::eR32G32B32A32Sfloat )
			.setOffset( offsetof(Vertex, color) );
		attributeDescriptions[2]
			.setLocation( 2 )
			.setBinding( 0 )
			.setFormat( vk::Format::eR32G32Sfloat )
			.setOffset( offsetof(Vertex, texCoord) );
		attributeDescriptions[3]
			.setLocation( 3 )
			.setBinding( 0 )
			.setFormat( vk::Format::eR32G32B32Sfloat )
			.setOffset( offsetof(Vertex, normal) );
		attributeDescriptions[4]
			.setLocation( 4 )
			.setBinding( 0 )
			.setFormat( vk::Format::eR32G32B32Sfloat )
			.setOffset( offsetof(Vertex, tangent) );
		
		return attributeDescriptions;
	}
	
	bool operator ==(const Vertex &other) const {
		
		return position == other.position && color == other.color && texCoord == other.texCoord
				&& normal == other.normal && tangent == other.tangent;
	}
};

// vertex data of the mesh loaded from file and stored in resource manager
struct MeshData{

    Array<Vertex> vertices;
    Array<U32> indices;
};


// mesh reference used to build indirect draw commmands
struct Mesh {

	// pointer to mesh data provided from ResourceManager
    MeshData *meshData;

	// ranges the mesh occupies in the Renderers combined vertex and index buffers
    U32 firstVertex;
    U32 vertexCount;
    U32 firstIndex;
    U32 indexCount;
};


} // Atuin


// custom hash for Vertex type
namespace std {
	template<> 
	struct hash<Atuin::Vertex> {
			
		size_t operator()(Atuin::Vertex const &vertex) const {
				
			return ((((hash<glm::vec3>()(vertex.position) ^
				     (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
					 (hash<glm::vec2>()(vertex.texCoord) << 1) >> 1) ^
					 (hash<glm::vec3>()(vertex.normal) << 1) >> 1) ^
					 (hash<glm::vec3>()(vertex.tangent) << 1);
		}
	};
}