
#pragma once


namespace Atuin {


class EngineLoop {

public:

    EngineLoop();
    ~EngineLoop();

    void Run();
    void StartUp();
    void ShutDown();

    bool isRunning() const { return mRunning; }
    void Quit() { mRunning = false; }

    void Update();
    void FixedUpdate();
    void VariableUpdate();


private:

    bool mRunning;

};


}; // Atuin