
#include "EngineLoop.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Files/FileManager.h"

#include <iostream>


namespace Atuin {


CVar<U32>* EngineLoop::pMaxFps           = ConfigManager::RegisterCVar("MAX_FPS", 30U);
CVar<U32>* EngineLoop::pMaxSimPerFrame   = ConfigManager::RegisterCVar("MAX_SIM_PER_FRAME", 1U);


EngineLoop::EngineLoop() : mRunning {false} {

    // TODO allocate on memory manager instead
    pFiles = new FileManager(this);
    pConfig = new ConfigManager(this);

    // read engine config file
    // TODO have separate cinfig files for engine and game (and key bindings)
    pConfig->Read("AtuinEngine/config.ini");
}


EngineLoop::~EngineLoop() {

    // TODO call dtors before deleting memomy manager
    delete pFiles;
    delete pConfig;
}


void EngineLoop::Run() {

    StartUp();

    std::cout << pMaxFps->Get() << '\n';
    std::cout << pMaxSimPerFrame->Get() << '\n';

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