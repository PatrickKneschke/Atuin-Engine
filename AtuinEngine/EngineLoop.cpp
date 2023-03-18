
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
    
}


void EngineLoop::ShutDown() {

}


void EngineLoop::Update() {

}


void EngineLoop::FixedUpdate() {

}


void EngineLoop::VariableUpdate() {

}


}; // Atuin