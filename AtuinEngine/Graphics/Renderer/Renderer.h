
#pragma once


#include "Definitions.h"
#include "Renderpass.h"
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


class RendererCore;

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

    void CreateVertexBuffer();
    void CreateIndexBuffer();

    Buffer CreateStagingBuffer(Size bufferSize);
    void UploadBufferData( void *bufferData, Size size, vk::Buffer targetBuffer, Size offset = 0);
    void TransitionImageLayout(vk::Image image, vk::ImageLayout initialLayout, vk::ImageLayout finalLayout, U32 mipLevels = 1);
    void CopyBufferToImage(vk::Buffer buffer, vk::Image image, U32 imageWidth, U32 imageHeight);
    void CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, Size offset, Size bufferSize);

    FrameResources& CurrentFrame() { return mFrames[mFrameCount % pFrameOverlap->Get()]; }

    void CreateFrameResources();
    void CreateSubmitContexts();
    void CreateShaderModules();
    void CreateSamplers();
    void CreateDescriptorResources();
    void CreateImageResource(ImageResource &image, std::string_view path, vk::Format format = vk::Format::eR8G8B8A8Unorm);
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

    DeletionStack mDeletionStack;

    ImmediateSubmitContext mGraphicsSubmit;
    ImmediateSubmitContext mTransferSubmit;

    Swapchain mSwapchain;
    ImageResource mDepthImage;

    // TODO shadow pass, opaque pass, transparency pass, deferred pass
    Renderpass mShadowPass;
    Renderpass mOpaquePass;
    Renderpass mTransparentPass;

    vk::RenderPass mRenderPass;
    Array<vk::Framebuffer> mFramebuffers; 

    Array<FrameResources> mFrames;
    U64 mFrameCount;

    Buffer mVertexBuffer;
    Buffer mIndexBuffer;

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

    ImageResource mMaterialAlbedoImage;
    ImageResource mMaterialNormalImage;
    ImageResource mMaterialMetallicImage;
    ImageResource mMaterialRoughnessImage;
    ImageResource mMaterialAoImage;

    

};


} // Atuin
