
#include "EngineLoop.h"

#include <iostream>


namespace Atuin {


EngineLoop::EngineLoop() : mRunning {false} {

}


EngineLoop::~EngineLoop() {

}


void EngineLoop::Run() {

        std::cout << "Run\n";

    StartUp();

    int i = 0;
    while (mRunning)
    {
        Update();

        std::cout << gameClock.ElapsedTime() << '\n';
        std::cout << gameClock.ElapsedFrames() << '\n';

        if(++i == 10000) {

            Quit();
        }

    }
    
    ShutDown();
}


void EngineLoop::StartUp() {
    
        std::cout << "Startup\n";

    gameClock.Reset();
    mRunning = true;
}


void EngineLoop::ShutDown() {

        std::cout << "ShutDown\n";
}


void EngineLoop::Update() {

        std::cout << "Update\n";

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

        std::cout << "Quit\n";

    mRunning = false;
}


} // Atuin