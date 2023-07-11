
#include "App.h"
#include "EngineLoop.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Files/FileManager.h"
#include "Core/Debug/Logger.h"
#include "Core/Memory/MemoryManager.h"
#include "Core/Jobs/JobManager.h"


namespace Atuin {


App* App::sInstance = nullptr;


void App::Quit() {

    if (sInstance != nullptr)
    {
        sInstance->ShutDown();
    }
}


App::App() {

    sInstance = this;

    pFiles  = new FileManager();
    pLog    = new Logger();

    pConfig = new ConfigManager();
    pConfig->Read("AtuinEngine/config.ini");

    pMemory = new MemoryManager();
    pJobs   = new JobManager();

    pEngine = new EngineLoop();
}


App::~App() {

    delete pEngine;

    delete pLog;
    delete pConfig;
    delete pFiles;
    delete pJobs;
    delete pMemory;
}
   

void App::StartUp() {

    pLog->StartUp();
    pJobs->StartUp();

    try
    {
        pEngine->Run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        pEngine->ShutDown();
    }
}


void App::ShutDown() {

    if (pEngine != nullptr)
    {
        pEngine->Quit();
    }

    pConfig->Save();
    pLog->ShutDown();
    pJobs->ShutDown(); 
}


} // Atuin
