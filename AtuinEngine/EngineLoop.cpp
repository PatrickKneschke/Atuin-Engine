
#include "EngineLoop.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Files/FileManager.h"

#include <iostream>


namespace Atuin {


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