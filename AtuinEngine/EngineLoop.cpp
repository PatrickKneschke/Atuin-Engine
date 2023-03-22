
#include "EngineLoop.h"

#include <iostream>


namespace Atuin {


EngineLoop::EngineLoop() : mRunning {false} {

}


EngineLoop::~EngineLoop() {

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

    gameClock.Reset();
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