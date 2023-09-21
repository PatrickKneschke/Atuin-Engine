
#include "EngineLoop.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Files/FileManager.h"
#include "Core/Jobs/JobManager.h"
#include "Core/Debug/Logger.h"
#include "Core/Memory/MemoryManager.h"
#include "Core/Util/StringFormat.h"
#include "Graphics/WindowModule.h"
#include "Graphics/RenderModule.h"
#include "Input/InputModule.h"


#include <iostream>


/*  just for testing  */

void testInputHandler(Atuin::MappedInput &input) {

    // continous action -> only check current state
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
    
    // ranges use value as is
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


EngineLoop::EngineLoop() : mRunning {false}, mLog(), mMemory(), mJobs()  {

    // engine modules
    pWindowModule = mMemory.New<WindowModule>();
    pInputModule  = mMemory.New<InputModule>();
    pRenderModule = mMemory.New<RenderModule>();
}


EngineLoop::~EngineLoop(){

    mMemory.Delete(pRenderModule);
    mMemory.Delete(pInputModule);
    mMemory.Delete(pWindowModule);
}


void EngineLoop::Run() {

    StartUp();

    U64 frame = 0;
    double prevTime = 0, currTime = 0;
    while (mRunning)
    {
        Update();

        ++frame;

        mGameClock.Update();
        currTime = mGameClock.ElapsedUnscaledTime();
        if ( currTime - prevTime >= 1.0)
        {
            std::cout << frame / (currTime - prevTime) << '\n';
            prevTime = currTime;
            frame = 0;
        }
    }

    ShutDown();
}


void EngineLoop::StartUp() {

    pWindowModule->StartUp();
    pInputModule->StartUp( pWindowModule->Window() );
    pRenderModule->StartUp( pWindowModule->Window() );


    // pInputModule->PushContext(SID("GamePlayContext"));
    // pInputModule->SetInputCallback(testInputHandler);


    mGameClock.Start();
    mRunning = true;
}


void EngineLoop::ShutDown() {

    pRenderModule->ShutDown();
    pInputModule->ShutDown();
    pWindowModule->ShutDown();
}


void EngineLoop::Update() {

    mGameClock.Update();

    pInputModule->Update();
    pWindowModule->Update();
    pRenderModule->Update();
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