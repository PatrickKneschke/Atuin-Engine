
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


/*  just for testing  */

void testInputHandler(Atuin::MappedInput &input) {

    // continous action -> only check curretn state
    if (input[Atuin::SID("CameraLeft")].value == Atuin::InputState::PRESS)
    {
        std::cout << "Move cam left\n";
    }
    if (input[Atuin::SID("CameraRight")].value == Atuin::InputState::PRESS)
    {
        std::cout << "Move cam right\n";
    }
    if (input[Atuin::SID("CameraForward")].value == Atuin::InputState::PRESS)
    {
        std::cout << "Move cam forward\n";
    }
    if (input[Atuin::SID("CameraBack")].value == Atuin::InputState::PRESS)
    {
        std::cout << "Move cam back\n";
    }

    // one off actions also check prev state
    if (input[Atuin::SID("Fire")].value == Atuin::InputState::PRESS && input[Atuin::SID("Fire")].prev == Atuin::InputState::RELEASE)
    {
        std::cout << "Fire!!!\n";
    }
    
    double dYaw = input[Atuin::SID("CameraYaw")].value;
    if (dYaw != 0)
    {
        std::cout << "Change yaw by " << dYaw << '\n';
    }
    
    double dPitch = input[Atuin::SID("CameraPitch")].value;
    if (dYaw != 0)
    {
        std::cout << "Change pitch by " << dPitch << '\n';
    }
}

/*  just for testing END */



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


    pInputModule->PushContext(SID("GamePlayContext"));
    pInputModule->SetInputCallback(testInputHandler);


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