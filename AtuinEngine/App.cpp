
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

    pLog->StartUp();
    pJobs->StartUp();

    pConfig->Read("AtuinEngine/config.ini");

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


void App::Quit() {

    if (pEngine != nullptr)
    {
        pEngine->Quit();
    }

    pConfig->Save();
    pLog->ShutDown();
    pJobs->ShutDown(); 
}


} // Atuin
