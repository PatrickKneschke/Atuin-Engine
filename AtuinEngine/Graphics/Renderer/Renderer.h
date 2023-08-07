
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

    void CreateDescriptorSetLayouts();
    void CreateShaderModules();
    void CreatePipeline();


    Array<char> readFile(const std::string &filename) {

        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if(!file.is_open()) {
            throw std::runtime_error("Failed to open file : " + filename);
        }
        
        size_t fileSize = (size_t) file.tellg();        
        Array<char> buffer;
        buffer.Resize(fileSize);

        file.seekg(0);
        file.read(buffer.Data(), fileSize);
        file.close();
        
        return buffer;
    }


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

    // TODO test pipeline, change later 
    Pipeline mSingleMaterialPipeline;
    vk::DescriptorSetLayout mCameraDataLayout;
    vk::DescriptorSetLayout mMaterialDataLayout;
    vk::DescriptorSetLayout mObjectDataLayout;
    vk::ShaderModule mMeshVertShader;
    vk::ShaderModule mMaterialFragShader;
    
};


} // Atuin
