
#pragma once


#include "Core/Time/Clock.h"
#include "Core/Config/CVar.h"


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

    // TODO direct access to private members might be problematic -> interface access ?
    // engine module access
    ConfigManager*  Config() const { return pConfig; }
    FileManager*    Files() const { return pFiles; }
    Logger*         Log() const  { return pLogger; }
    MemoryManager*  Memory() const { return pMemory; }
    JobManager*     Jobs() const { return pJobs; }


private:

    // config variables
    static CVar<U32>* pMaxFps;
    static CVar<U32>* pMaxSimPerFrame;

    bool mRunning;
    Clock mGameClock;

    // engine systems
    ConfigManager*  pConfig;
    FileManager*    pFiles;
    Logger*         pLogger;
    MemoryManager*  pMemory;
    JobManager*     pJobs;

    // engine modules
    WindowModule*   pWindowModule;
    InputModule*    pInputModule;
};


} // Atuin