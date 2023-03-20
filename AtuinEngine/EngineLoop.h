
#pragma once


#include "Core/Time/Clock.h"


namespace Atuin {


class EngineLoop {

public:

    EngineLoop();
    ~EngineLoop();

    void Run();
    void StartUp();
    void ShutDown();

    bool isRunning() const;
    void Quit();

    void Update();
    void FixedUpdate();
    void VariableUpdate();


private:

    bool mRunning;
    Clock gameClock;
};


} // Atuin