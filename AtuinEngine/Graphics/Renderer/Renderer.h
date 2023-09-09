
#pragma once


#include "Definitions.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshPass.h"
#include "Core/Config/CVar.h"
#include "Core/Debug/Log.h"
#include "Core/Memory/Memory.h"
#include "Core/Jobs/Jobs.h"
#include "Core/Files/Files.h"
#include "Core/DataStructures/Array.h"
#include "Core/DataStructures/Map.h"

#include <functional>



class GLFWwindow;

namespace Atuin {


class DeletionStack {

public:

    void Push( std::function<void()> &&func) {

        mDeletors.PushBack( std::move(func));
    }

    void Flush() {

        while ( !mDeletors.IsEmpty())
        {
            mDeletors.Back()();
            mDeletors.PopBack();
        }
    }

private:

    Array<std::function<void()>> mDeletors;
};


struct FrameResources {

	vk::Fence renderFence;
	vk::Semaphore renderSemaphore;
	vk::Semaphore presentSemaphore;

	vk::CommandPool commandPool;
	vk::CommandBuffer commandBuffer;

    // DeletionStack deletionStack;
};

// mesh + material combination as stored in the ModelComponent of Entities
struct MeshObject {

    glm::mat4 transform;
    glm::vec4 sphereBounds;

    // Mesh* mesh;
    // Material* material;
    std::string meshName;
    std::string materialName;
};

// representation of a MeshObject inside the Renderer
struct RenderObject {

    glm::mat4 transform;
    glm::vec4 sphereBounds;

    U64 meshId;
    U64 materialId;

    PassData<I64> passIndex;
};


class RendererCore;
class ResourceManager;

class Renderer {

public:

    Renderer();
    ~Renderer();

    void StartUp(GLFWwindow* window);
    void ShutDown();
    void Update();


private:

	void CreateDepthResources();
    void CreateRenderPasses();
    void CreateFramebuffers();
    void DestroyFramebuffers();
    void RecreateSwapchain();

    void RegisterMeshObject( const MeshObject &object);
    U64 RegisterMesh( std::string_view meshName);
    U64 RegisterMaterial( std::string_view materialName);
    void CreateMesh( std::string_view meshName);
    void CreateMaterial( std::string_view materialName);
    void CreateTexture( std::string_view textureName);
    void CreatePipeline( std::string_view pipelineName);

    void MergeMeshes();
    void UpdateMeshPass( MeshPass *pass);
    void BuildMeshPassBatches( MeshPass *pass);
    void UpdateMeshPassBatchBuffer( MeshPass *pass);
    void UpdateMeshPassInstanceBuffer( MeshPass *pass);
    void UpdateObjectBuffer();

    void CreateBuffer( Buffer &buffer, Size size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memoryType);

    void CreateVertexBuffer();
    void CreateIndexBuffer();


    Buffer CreateStagingBuffer(Size bufferSize);
    void UploadBufferData( void *bufferData, Size size, vk::Buffer targetBuffer, Size offset = 0);
    void TransitionImageLayout(vk::Image image, vk::ImageLayout initialLayout, vk::ImageLayout finalLayout, U32 mipLevels = 1);
    void CopyBufferToImage(vk::Buffer buffer, vk::Image image, U32 imageWidth, U32 imageHeight);
    void CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, Size offset, Size bufferSize);

    FrameResources& CurrentFrame() { return mFrames[mFrameCount % pFrameOverlap->Get()]; }

    void CreateSubmitContexts();
    void CreateFrameResources();
    void CreateShaderModules();
    void CreateSamplers();

    void CreateDescriptorResources();
    void CreateImage(Image &image, std::string_view path, vk::Format format = vk::Format::eR8G8B8A8Unorm);
    void LoadModel(std::string_view path);

    void CreateDescriptorSetLayouts();
    void CreateDescriptorPool();
    void CreateDescriptorSets();    

    void CreatePipeline();

    void DrawFrame();
    void UpdateCameraData();
    void UpdateScenedata();
    void UpdateObjectData();


    static CVar<U32>* pFrameOverlap; 


    Log mLog;
    Memory mMemory;
    Jobs mJobs;
    Files mFiles;

    GLFWwindow* pWindow;
    RendererCore* pCore;
    ResourceManager* pResources;

    DeletionStack mDeletionStack;

    ImmediateSubmitContext mGraphicsSubmit;
    ImmediateSubmitContext mTransferSubmit;

    Swapchain mSwapchain;
    Image mDepthImage;

    // render passes and framebuffers

    vk::RenderPass mForwardPass;
    vk::RenderPass mShadowpass;

    Array<vk::Framebuffer> mForwardFramebuffers;
    Array<vk::Framebuffer> mShadowFramebuffers;

    Array<FrameResources> mFrames;
    U64 mFrameCount;

    // holds all the vertex data of unique meshes in the scene
    Buffer mCombinedVertexBuffer;
    // holds all the index data of unique meshes in the scene
    Buffer mCombinedIndexBuffer;

    // list of all objects to be rendered
    Array<RenderObject> mRenderObjects;
    // indices of objects that have been updated since last frame
    Array<U32> mDirtyObjectIndices;

    // render resource caches
    Map<U64, Mesh> mMeshes;
    Map<U64, Material> mMaterials;
    Map<U64, Image> mTextures;
    Map<U64, Pipeline> mPipelines;

    // // list of all meshes in use
    // Array<Mesh*> mMeshes;
    // Map<Mesh*, U32> mMeshIndices; 
    // // list of all materials in use
    // Array<Material*> mMaterials;
    // Map<Material*, U32> mMaterialIndices;

    // mesh passes
    MeshPass mShadowMeshPass;
    MeshPass mOpaqueMeshPass;
    MeshPass mTransparentMeshPass;

    // TODO use dynamic uniform buffer or move into FrameResource ?
    Buffer mCameraBuffer;
    Buffer mSceneBuffer;
    Buffer mObjectBuffer;
    
    vk::Sampler mSampler;


    Array<Vertex> mVertices;
    Array<U32> mIndices;


    // TODO test pipeline to render single material and mesh at a time, change later 
    Pipeline mSingleMaterialPipeline;
    vk::ShaderModule mMeshVertShader;
    vk::ShaderModule mMaterialFragShader;

    vk::DescriptorSetLayout mPassDataLayout;
    vk::DescriptorSetLayout mMaterialDataLayout;
    vk::DescriptorSetLayout mObjectDataLayout;

    vk::DescriptorPool mDescriptorPool;
    vk::DescriptorSet mPassDataSet;
    vk::DescriptorSet mMaterialDataSet;
    vk::DescriptorSet mObjectDataSet;

    Image mMaterialAlbedoImage;
    Image mMaterialNormalImage;
    Image mMaterialMetallicImage;
    Image mMaterialRoughnessImage;
    Image mMaterialAoImage;

    

};


} // Atuin
