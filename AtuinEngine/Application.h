
#pragma once


#include "EngineLoop.h"

#include <memory>
#include <stdexcept>


namespace Atuin {


class Application {

public:

    static void Start() {

        if (!sEngineLoop)
        {
            sEngineLoop.reset( new EngineLoop() );
        }

        if (sEngineLoop->isRunning())
        {
            throw std::runtime_error("Cannot start engine after it has already been started!");
        }

        sEngineLoop->Run();
    }

    static void Quit()  {

        sEngineLoop->Quit(); 
    }


private:

    static std::unique_ptr<EngineLoop> sEngineLoop;
};


std::unique_ptr<EngineLoop> Application::sEngineLoop;


} // Atuin