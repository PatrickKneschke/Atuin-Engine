
#pragma once


namespace Atuin {
    

class ConfigManager;
class EngineLoop;
class FileManager;
class JobManager;
class Logger;
class MemoryManager;

class App {

public: 

    static void Quit();

    App();
    App(const App &other) = delete;
    App(App &&other) = delete;
    App& operator= (const App &other) = delete;
    App& operator= (App &&other) = delete;
    ~App();

    void StartUp();
    void ShutDown();


private:

    static App* sInstance;

    FileManager*   pFiles;
    ConfigManager* pConfig;
    Logger*        pLog;
    MemoryManager* pMemory;
    JobManager*    pJobs;
    EngineLoop*    pEngine;
};


} // Atuin

