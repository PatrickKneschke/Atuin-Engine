
#pragma once


#include "Core/Time/Clock.h"
#include "Core/Config/CVar.h"
#include "Core/Debug/Log.h"
#include "Core/Jobs/Jobs.h"
#include "Core/Memory/Memory.h"


namespace Atuin {


class ConfigManager;
class FileManager;
class Logger;
class MemoryManager;
class JobManager;

class WindowModule;
class InputModule;

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

    // config variables
    static CVar<U32>* pMaxFps;
    static CVar<U32>* pMaxSimPerFrame;

    bool mRunning;
    Clock mGameClock;

    Log mLog;
    Memory mMemory;
    Jobs mJobs;

    // engine modules
    WindowModule*   pWindowModule;
    InputModule*    pInputModule;
};


} // Atuin