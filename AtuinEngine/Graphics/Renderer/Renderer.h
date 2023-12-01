
#pragma once


#include "VulkanInclude.h"
#include "Definitions.h"
#include "Material.h"
#include "Mesh.h"
#include "MeshPass.h"
#include "Descriptors.h"
#include "Pipeline.h"
#include "Camera.h"
#include "Light.h"
#include "Core/Util/Types.h"
#include "Core/Config/CVar.h"
#include "Core/Debug/Log.h"
#include "Core/Memory/Memory.h"
#include "Core/Jobs/Jobs.h"
#include "Core/Files/Files.h"
#include "Core/DataStructures/Array.h"
#include "Core/DataStructures/Map.h"

#include <functional>
#include <mutex>


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

    DeletionStack deletionStack;
    DescriptorSetAllocator *descriptorAllocator;
};

// mesh + material combination as stored in the ModelComponent of Entities
struct MeshObject {

    glm::mat4 transform;
    glm::vec4 sphereBounds;

    std::string meshName;
    std::string materialName;

    U32 objectIdx;
};

// representation of a MeshObject inside the Renderer
struct RenderObject {

    // TODO is invalidated if mesh objects move after reallocation!
    glm::mat4 *transform;
    glm::vec4 *sphereBounds;

    U64 meshId;
    U64 materialId;

    PassData<I64> passIndex;
    bool updated = true;
};


struct CameraData {

    glm::mat4 view = glm::mat4(1.f);
    glm::mat4 proj = glm::mat4(1.f);
    glm::mat4 viewProj = glm::mat4(1.f);
    glm::vec4 position = glm::vec4(0.f);
};


struct SceneData {

    glm::vec3 ambientColor;
    float ambientIntensity;
    glm::vec3 sunColor; 
    float sunIntensity;
    glm::vec3 sunDirection;
    // TODO add fog
};


struct ViewCullData {

	glm::mat4 view; // camera view matrix
	
	float P00, P11, zNear, zFar; // symmetric projection parameters
	float frustum[4]; // data for left/right/top/bottom frustum planes
	float pyramidWidth, pyramidHeight; // depth pyramid size in texels

	U32 drawCount;
	U32 enableDistCull;
	U32 enableOcclusionCull;
};


struct DirectionalCullData {
    
	uint drawCount;
	uint cascadeCount;
};


struct CullDebugData {

    glm::vec4 aabb;
    float depth;
    float sampledDepth;
    float level;
    U32 culled;
};


struct AsyncBufferCopyData {

    Array<vk::BufferCopy> bufferCopies;
    Buffer stagingBuffer;
    vk::Buffer targetBuffer;
    MeshPass* pass = nullptr;
};


class RendererCore;
class ResourceManager;

class Renderer {

    friend class RenderModule;

public:

    Renderer();
    ~Renderer();

    void StartUp(GLFWwindow* window);
    void ShutDown();
    void Update();

    U64 FrameCount() { return mFrameCount; }


private:
    
    FrameResources& CurrentFrame() { return mFrames[ mFrameCount % pFrameOverlap->Get()]; }
    FrameResources& NextFrame() { return mFrames[ (mFrameCount + 1) % pFrameOverlap->Get()]; }

    // inits
    void CreateSubmitContexts();
    void CreateRenderPasses();
    void CreateFrameResources();
	void CreateDepthResources();
    void CreateShadowResources();
    void CreateFramebuffers();
    void DestroyFramebuffers();
    void RecreateSwapchain();
    void CreateDefaultPipelineBuilder();
    void CreateMeshPass( MeshPass *pass, PassType type);

    // resources
    void RegisterMeshObject( MeshObject *object);
    void DeleteMeshObject( MeshObject *object);
    void UpdateMeshObject( MeshObject *object);
    U64 RegisterMesh( std::string_view meshName);
    U64 RegisterMaterial( std::string_view materialName);
    void CreateMesh( std::string_view meshName);
    void CreateMaterial( std::string_view materialName);
    void CreateTexture( std::string_view textureName, vk::Format format = vk::Format::eR8G8B8A8Unorm);
    void CreateSampler( std::string_view samplerName);
    void CreatePipeline( std::string_view pipelineName, vk::RenderPass renderPass = vk::RenderPass{});
    void CreateShaderModule( std::string_view shaderName);

    // updates
    void MergeMeshes();
    void UpdateMeshPass( MeshPass *pass);
    void BuildMeshPassBatches( MeshPass *pass);
    void UpdateMeshPassBatchBuffer( AsyncBufferCopyData *data);
    void UpdateMeshPassInstanceBuffer( AsyncBufferCopyData *data);
    void UpdateObjectBuffer( AsyncBufferCopyData *data);
    void UpdateCameraData();
    void UpdateSceneData();
    void UpdateShadowCascade();

    // drawing
    void DrawFrame();
    void CullShadowPass( vk::CommandBuffer cmd);
    void CullForwardPass( vk::CommandBuffer cmd);
    void DrawShadowPass( vk::CommandBuffer cmd, U32 imageIndex);
    void DrawForwardPass( vk::CommandBuffer cmd, U32 imageIndex);
    void RenderMeshPass( vk::CommandBuffer cmd, MeshPass *pass, vk::DescriptorSet globalDataSet,   U32 pushConstantSize = 0, void *pushConstantData = nullptr, vk::ShaderStageFlagBits pushConstantStage = vk::ShaderStageFlagBits{});
    void UpdateDepthPyramid( vk::CommandBuffer cmd);

    // utils
    void CreateBuffer( Buffer &buffer, Size size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags memoryType);
    Buffer CreateStagingBuffer(Size bufferSize);
    // TODO split resource copy methods into synchronous ( using current command buffer in graphics queue) and async ( using transfer queue)
    void UploadBufferData( void *bufferData, Size size, vk::Buffer targetBuffer, Size offset = 0);
    void TransitionImageLayout(vk::Image image, vk::ImageLayout initialLayout, vk::ImageLayout finalLayout, U32 mipLevels = 1);
    void CopyBufferToImage(vk::Buffer buffer, vk::Image image, U32 imageWidth, U32 imageHeight);
    void CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, Size offset, Size bufferSize);

    void CreateDescriptorResources();
    void CreateDescriptorSetLayouts();
    void CreateDescriptorSets();    

    void CreateSamplers();
    void CreatePipelines();

    void FinishBufferUpdate( vk::CommandBuffer cmd, AsyncBufferCopyData *updateData);


    static CVar<U32>*   pFrameOverlap;
    static CVar<U32>*   pMaxAnisotropy;
    static CVar<U32>*   pMsaaSamples;
    static CVar<U32>*   pShadowCascadeCount;
    static CVar<float>* pShadowCascadeSplit;


    Log mLog;
    Memory mMemory;
    Jobs mJobs;
    Files mFiles;

    std::string mResourceDir;

    std::mutex mUpdateMutex;

    GLFWwindow* pWindow;
    RendererCore* pCore;
    ResourceManager* pResources;
    DescriptorSetAllocator* pDescriptorSetAllocator;
    DescriptorLayoutCache* pDescriptorLayoutCache;

    DeletionStack mDeletionStack;

    ImmediateSubmitContext mGraphicsSubmit;
    ImmediateSubmitContext mTransferSubmit;

    GraphicsPipelineBuilder mDefaultPipelineBuilder;

    Swapchain mSwapchain;
    Image mDepthImage;

    //depth pyramid image views
    vk::Format mDepthFormat = vk::Format::eD32Sfloat;
    Image mDepthPyramid;
    Array<vk::ImageView> mDepthPyramidViews;

    Pipeline mDepthReducePipeline;
    vk::Sampler mDepthSampler;


    // main light shadow image
    Image mShadowImage;
    vk::Extent3D mShadowExtent;
    Buffer mLightBuffer;
    vk::Sampler mShadowSampler;

    DirectionalLight mMainLight;


    // culling
    Pipeline mViewCullPipeline;
    Pipeline mShadowCullPipeline;

    Array<vk::BufferMemoryBarrier> mPreCullBarriers;
    Array<vk::BufferMemoryBarrier> mPostCullBarriers;

    // camera data
    Camera mMainCamera;
    
    // render passes and framebuffers

    vk::RenderPass mShadowPass;
    vk::RenderPass mForwardPass;

    Array<vk::Framebuffer> mShadowFramebuffers;
    Array<vk::Framebuffer> mForwardFramebuffers;

    Array<FrameResources> mFrames;
    U64 mFrameCount;

    // holds all the vertex data of unique meshes in the scene
    Buffer mCombinedVertexBuffer;
    // holds all the index data of unique meshes in the scene
    Buffer mCombinedIndexBuffer;

    // list of all objects to be rendered
    Array<RenderObject> mRenderObjects;
    // indices of objects that have been added or updated since last frame
    Array<U32> mDirtyObjectIndices;

    // indices of objects that can be overwritten by new objects added 
    Array<U32> mReuseObjectIndices;

    // flag to indicate changes in meshes
    bool mMeshesDirty;

    // render resource caches
    Map<U64, Mesh> mMeshes;
    Map<U64, Material> mMaterials;
    Map<U64, Image> mTextures;
    Map<U64, Pipeline> mPipelines;
    Map<U64, vk::Sampler> mSamplers;
    Map<U64, vk::ShaderModule> mShaders;

    // mesh passes
    MeshPass mShadowMeshPass;
    MeshPass mOpaqueMeshPass;
    MeshPass mTransparentMeshPass;

    // TODO use dynamic uniform buffer or move into FrameResource ?
    Buffer mCameraBuffer;
    Buffer mSceneBuffer;
    Buffer mObjectBuffer;
    
    vk::DescriptorSetLayout mGlobalDataLayout;
    vk::DescriptorSet mGlobalDataSet;


    Buffer cullDebugBuffer;
};


} // Atuin
