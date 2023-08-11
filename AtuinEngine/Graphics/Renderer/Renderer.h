
#pragma once


#include "Definitions.h"
#include "Core/Config/CVar.h"
#include "Core/Debug/Log.h"
#include "Core/Memory/Memory.h"
#include "Core/Jobs/Jobs.h"
#include "Core/Files/Files.h"

#include "Core/DataStructures/Array.h"
#include <fstream>


class GLFWwindow;

namespace Atuin {
    

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
    void CreateRenderPass();
    void CreateFramebuffers();

    void CreateVertexBuffer();
    void CreateIndexBuffer();

    Buffer CreateStagingBuffer(Size bufferSize);
    void UploadBufferData( void *bufferData, Size size, vk::Buffer targetBuffer);
    void TransitionImageLayout(vk::Image image, vk::ImageLayout initialLayout, vk::ImageLayout finalLayout, U32 mipLevels = 1);
    void CopyBufferToImage(vk::Buffer buffer, vk::Image image, U32 imageWidth, U32 imageHeight);
    void CopyBuffer(vk::Buffer srcBuffer, vk::Buffer dstBuffer, Size offset, Size bufferSize);

    FrameResources CurrentFrame() { return mFrames[mFrameCount % pFrameOverlap->Get()]; }

    void CreateFrameResources();
    void CreateSubmitContexts();
    void CreateShaderModules();
    void CreateSamplers();
    void CreateDescriptorResources();
    void CreateImageResource(ImageResource &image, std::string_view path);
    void LoadModel(std::string_view path);

    void CreateDescriptorSetLayouts();
    void CreateDescriptorPool();
    void CreateDescriptorSets();    

    void CreatePipeline();



    static CVar<U32>* pFrameOverlap; 


    Log mLog;
    Memory mMemory;
    Jobs mJobs;
    Files mFiles;

    GLFWwindow* pWindow;
    RendererCore* pCore;

    Swapchain mSwapchain;
    ImageResource mDepthImage;
    vk::RenderPass mRenderPass;
    Array<vk::Framebuffer> mFramebuffers; 

    Array<FrameResources> mFrames;
    U64 mFrameCount;

    ImmediateSubmitContext mGraphicsSubmit;
    ImmediateSubmitContext mTransferSubmit;

    Buffer mVertexBuffer;
    Buffer mIndexBuffer;

    Buffer mCameraBuffer;
    
    vk::Sampler mSampler;


    Array<Vertex> mVertices;
    Array<U32> mIndices;


    // TODO test pipeline to render single material and mesh at a time, change later 
    Pipeline mSingleMaterialPipeline;
    vk::ShaderModule mMeshVertShader;
    vk::ShaderModule mMaterialFragShader;

    vk::DescriptorSetLayout mCameraDataLayout;
    vk::DescriptorSetLayout mMaterialDataLayout;
    vk::DescriptorSetLayout mObjectDataLayout;

    vk::DescriptorPool mDescriptorPool;
    vk::DescriptorSet mCameraDataSet;
    vk::DescriptorSet mMaterialDataSet;
    vk::DescriptorSet mObjectDataSet;

    ImageResource mMaterialDiffuseImage;
    ImageResource mMaterialNormalImage;
    ImageResource mMaterialSpecularImage;
    Buffer mObjectBuffer;

};


} // Atuin
