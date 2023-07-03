
#pragma once


namespace Atuin
{
    

class ConfigManager;
class EngineLoop;
class FileManager;
class JobManager;
class Logger;
class MemoryManager;

class App {

public:

    static App& Get();

    static FileManager*   Files()  { return Get().pFiles; }
    static ConfigManager* Config() { return Get().pConfig; }
    static Logger*        Log()    { return Get().pLog; }
    static MemoryManager* Memory() { return Get().pMemory; }
    static JobManager*    Jobs()   { return Get().pJobs; }

    static EngineLoop*    Engine() { return Get().pEngine; }
    

    void Start();
    void Quit();


private:


    static App* sInstance;


    App();
    App(const App &other) = delete;
    App(App &&other) = delete;
    App& operator= (const App &other) = delete;
    App& operator= (App &&other) = delete;
    ~App();

    FileManager*   pFiles;
    ConfigManager* pConfig;
    Logger*        pLog;
    MemoryManager* pMemory;
    JobManager*    pJobs;
    EngineLoop*    pEngine;
};


} // Atuin

