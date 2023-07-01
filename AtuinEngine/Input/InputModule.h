
#pragma once


#include "InputSignals.h"
#include "RangeConverter.h"
#include "Core/DataStructures/Map.h"
#include "Core/DataStructures/Json.h"

// #include "json/json.hpp"

#include <functional>


class GLFWwindow;

namespace Atuin {


struct InputState {

    constexpr static double RELEASE = 0;
    constexpr static double PRESS = 1;

    double value = RELEASE;
    double prev = RELEASE;
};

using MappedInput = Map<U64, InputState>;
using InputHandler = std::function<void(MappedInput&)>;


class EngineLoop;
class InputContext;

class InputModule {

public:

    InputModule(EngineLoop *engine);
    ~InputModule();

    void StartUp(GLFWwindow *window);
    void ShutDown();
    void Update();

    void LoadContexts(std::string_view contextFilePath);
    void SaveContexts(std::string_view contextFilePath);
    void LoadRanges(std::string_view rangesFilePath);

    void PushContext(U64 contextID);
    void PopContext();
    void SetInputCallback(InputHandler callback);

    bool IsKeyPressed(int key) const;
    bool IsMouseButtonPressed(int button) const;
    std::pair<double, double> GetCursorPos() const;


private:

    static void KeyListener(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void MouseListener(GLFWwindow *window, int button, int action, int mods);
    static void CursorPosListener(GLFWwindow *window, double xpos, double ypos);

    static GLFWwindow* sWindow;

    void PushInputStates();
    
    Json mContextsJSON;

    Map<U64, InputContext*> mContexts;
    InputContext* pActiveContext;
    InputHandler mCallback;

    RangeConverter mRangeConverter;

    // tracks what input a signal corresponds to given the active context(s)
    U64 mSignalMap[(Size)Signal::COUNT];
    // stores the states of all relevant inputs
    MappedInput mCurrentMappedInput;
    double mouseX, mouseY;

    EngineLoop* pEngine;
};

    
} // Atuin
