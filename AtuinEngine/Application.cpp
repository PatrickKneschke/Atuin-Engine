
#include "Application.h"
#include "EngineLoop.h"

#include <stdexcept>


namespace Atuin {


std::unique_ptr<EngineLoop> Application::sEngineLoop = nullptr;


void Application::Start() {

    if (!sEngineLoop) {

        sEngineLoop.reset( new EngineLoop() );
    }

    if (sEngineLoop->isRunning())
    {
        throw std::runtime_error("Cannot start engine after it has already been started!");
    }

    sEngineLoop->Run();
}


void Application::Quit() {

    sEngineLoop->Quit(); 
}


} // Atuin