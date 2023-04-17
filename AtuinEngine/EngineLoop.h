
#pragma once


#include "Core/Time/Clock.h"
#include "Core/Config/CVar.h"


namespace Atuin {


class ConfigManager;
class FileManager;
class Logger;
class MemoryManager;

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

    // engine module access
    ConfigManager*  Config() const { return pConfig; }
    FileManager*    Files() const { return pFiles; }
    Logger*         Log() const  { return pLogger; }
    MemoryManager*  Memory() { return pMemory; }


private:

    // config variables
    static CVar<U32>* pMaxFps;
    static CVar<U32>* pMaxSimPerFrame;

    bool mRunning;
    Clock gameClock;

    // engine modules
    ConfigManager*  pConfig;
    FileManager*    pFiles;
    Logger*         pLogger;
    MemoryManager*  pMemory;
};


} // Atuin