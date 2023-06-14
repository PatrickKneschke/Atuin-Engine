
#include "EngineLoop.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Files/FileManager.h"
#include "Core/Jobs/JobManager.h"
#include "Core/Debug/Logger.h"
#include "Core/Memory/MemoryManager.h"
#include "Core/Util/StringFormat.h"
#include "Graphics/WindowModule.h"
#include "Input/InputModule.h"

#include <iostream>


namespace Atuin {


CVar<U32>* EngineLoop::pMaxFps           = ConfigManager::RegisterCVar("Engine Loop", "MAX_FPS", 30U);
CVar<U32>* EngineLoop::pMaxSimPerFrame   = ConfigManager::RegisterCVar("Engine Loop", "MAX_SIM_PER_FRAME", 1U);


EngineLoop::EngineLoop() : mRunning {false} {

    pFiles = new FileManager(this);
    pConfig = new ConfigManager(this);
    pLogger = new Logger(this);

    // read engine config file
    // TODO (optional) have separate config files for engine and game (and key bindings)
    pConfig->Read("AtuinEngine/config.ini");

    pMemory = new MemoryManager(this);
    pJobs = new JobManager(this);

    // engine modules
    pWindowModule = pMemory->New<WindowModule>(this);
    pInputModule  = pMemory->New<InputModule>(this);
}


EngineLoop::~EngineLoop() {

    pMemory->Delete(pInputModule);
    pMemory->Delete(pWindowModule);

    // TODO call dtors before deleting memomy manager
    delete pLogger;
    delete pConfig;
    delete pFiles;
    delete pJobs;
    delete pMemory;
}


void EngineLoop::Run() {

    StartUp();

    int frame = 0;
    while (mRunning)
    {
        Update();

        ++frame;
    }

    ShutDown();
}


void EngineLoop::StartUp() {

    pLogger->StartUp();
    pJobs->StartUp();

    pWindowModule->StartUp();
    pInputModule->StartUp(pWindowModule->Window());

    mGameClock.Start();
    mRunning = true;
}


void EngineLoop::ShutDown() {

    pInputModule->ShutDown();
    pWindowModule->ShutDown();

    pConfig->Save();
    pLogger->ShutDown();
    pJobs->ShutDown();
}


void EngineLoop::Update() {

    mGameClock.Update();

    pInputModule->Update();
    pWindowModule->Update();
}


void EngineLoop::FixedUpdate() {

}


void EngineLoop::VariableUpdate() {

}


bool EngineLoop::IsRunning() const { 
    
    return mRunning; 
}


void EngineLoop::Quit() {

    mRunning = false;
}


} // Atuin