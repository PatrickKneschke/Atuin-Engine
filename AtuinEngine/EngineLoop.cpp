
#include "EngineLoop.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Files/FileManager.h"
#include "Core/Debug/Logger.h"

#include <iostream>


namespace Atuin {


CVar<U32>* EngineLoop::pMaxFps           = ConfigManager::RegisterCVar("Engine Loop", "MAX_FPS", 30U);
CVar<U32>* EngineLoop::pMaxSimPerFrame   = ConfigManager::RegisterCVar("Engine Loop", "MAX_SIM_PER_FRAME", 1U);


EngineLoop::EngineLoop() : mRunning {false} {

    // TODO allocate on memory manager instead
    pFiles = new FileManager(this);
    pConfig = new ConfigManager(this);
    pLogger = new Logger(this);

    // read engine config file
    // TODO (optional) have separate cinfig files for engine and game (and key bindings)
    pConfig->Read("AtuinEngine/config.ini");

    pLogger->Error("Test Error Message");
}


EngineLoop::~EngineLoop() {

    // TODO call dtors before deleting memomy manager
    delete pLogger;
    delete pConfig;
    delete pFiles;
}


void EngineLoop::Run() {

    StartUp();

    int i = 0;
    while (mRunning)
    {
        Update();

        if(++i == 100) {

            Quit();
        }

    }
    
    ShutDown();
}


void EngineLoop::StartUp() {

    gameClock.Start();
    mRunning = true;
}


void EngineLoop::ShutDown() {

    pConfig->Save();
}


void EngineLoop::Update() {

    gameClock.Update();
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