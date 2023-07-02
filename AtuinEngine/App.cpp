
#include "App.h"
#include "EngineLoop.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Files/FileManager.h"
#include "Core/Debug/Logger.h"
#include "Core/Memory/MemoryManager.h"
#include "Core/Jobs/JobManager.h"


namespace Atuin {


App::App() {

    pFiles  = new FileManager( pEngine );
    pConfig = new ConfigManager( pEngine );
    pLog    = new Logger( pEngine );
    pMemory = new MemoryManager( pEngine );
    pJobs   = new JobManager( pEngine );

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


App& App::Get() {

    static App instance;

    return instance;
}
    

void App::Start() {

    Get().Log()->StartUp();
    Get().Jobs()->StartUp();

    Get().Config()->Read("AtuinEngine/config.ini");

    try
    {
        Get().Engine()->Run();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        Get().Engine()->ShutDown();
    }
}


void App::Quit() {

    if (Get().Engine() != nullptr)
    {
        Get().Engine()->Quit();
    }

    Get().Config()->Save();
    Get().Log()->ShutDown();
    Get().Jobs()->ShutDown(); 
}


} // Atuin
