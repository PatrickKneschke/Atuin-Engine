
#pragma once


#include "Core/Time/Clock.h"

#include "Core/Util/Types.h"
#include "Core/Util/StringID.h"

    

namespace Atuin {


class EngineLoop {

public:

    EngineLoop();
    ~EngineLoop();

    void Run();
    void StartUp();
    void ShutDown();

    bool IsRunning() const;
    void Quit();

    void Update();
    void FixedUpdate();
    void VariableUpdate();


private:

    bool mRunning;
    Clock gameClock;
};


} // Atuin