
#pragma once


#include "InputSignals.h"
#include "RangeConverter.h"

#include "json/json.hpp"

#include <functional>

#include <list>
#include <unordered_map>


class GLFWwindow;

namespace Atuin {


using MappedInput = std::unordered_map<U64, std::pair<double,double>>; // input -> (state, prev state) 
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
    
    json::JSON mContextsJSON; // TODO use own JSON class

    std::unordered_map<U64, InputContext*> mContexts; // TODO replace with custom map
    InputContext* pActiveContext;
    InputHandler mCallback;

    RangeConverter mRangeConverter;

    // Array<U64> mSignalMap;
    U64 mSignalMap[(Size)Signal::COUNT];
    MappedInput mCurrentMappedInput;
    double mouseX, mouseY;

    EngineLoop* pEngine;
};

    
} // Atuin
