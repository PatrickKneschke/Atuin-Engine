
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

        if(++i == 10) {

            Quit();
        }

    }
    
    ShutDown();
}


void EngineLoop::StartUp() {
    
    gameClock.Reset();
}


void EngineLoop::ShutDown() {

}


void EngineLoop::Update() {

}


void EngineLoop::FixedUpdate() {

}


void EngineLoop::VariableUpdate() {

}


bool EngineLoop::isRunning() const { 
    
    return mRunning; 
}


void EngineLoop::Quit() {

    mRunning = false;
}


} // Atuin