
#include "InputModule.h"
#include "GLFWInput.h"
#include "InputContext.h"
#include "EngineLoop.h"
#include "Core/Files/FileManager.h"
#include "Core/Memory/MemoryManager.h"
#include "Core/Util/StringID.h"

#include "GLFW/glfw3.h"

#include <iostream>


namespace Atuin {


GLFWwindow* InputModule::sWindow = nullptr;


InputModule::InputModule(EngineLoop *engine) : pActiveContext {nullptr}, pEngine {engine} {

}


InputModule::~InputModule() {

}


void InputModule::StartUp(GLFWwindow *window) {

    sWindow = window;
    
    glfwSetWindowUserPointer(sWindow, this);

    glfwSetKeyCallback(sWindow, KeyListener);
    glfwSetMouseButtonCallback(sWindow, MouseListener);
    glfwSetCursorPosCallback(sWindow, CursorPosListener);

    // TODO config variable for file paths ?
    LoadContexts("../../Resources/input_contexts.json");
    LoadRanges("../../Resources/input_ranges.json");
}


void InputModule::ShutDown() {

    for (auto [name, context] : mContexts)
    {
        pEngine->Memory()->Delete(context);
    }
    
}


void InputModule::Update() {

    PushInputStates();
    glfwPollEvents();
    if (mCallback)
    {
        mCallback(mCurrentMappedInput);
    }
};


void InputModule::PushInputStates() {

    for(auto &[input, state] : mCurrentMappedInput)
    {
        state.prev = state.value;
        state.value = InputState::RELEASE;
    }
}


void InputModule::LoadContexts(std::string_view contextFilePath) {

    // TODO use own JSON class

    std::string jsonTxt(pEngine->Files()->Read(contextFilePath));
    mContextsJSON = json::JSON::Load(jsonTxt);
    auto contexts = mContextsJSON.ObjectRange();

    for(auto &[name, contextData] : contexts) 
    {
        U64 nameID = SID(name.c_str());
        mContexts[nameID] = pEngine->Memory()->New<InputContext>(name); 

        auto inputMap = contextData.ObjectRange();
        for(auto &[input, signal] : inputMap) 
        {
            mContexts[nameID]->MapSignal(StringToSignal(signal.ToString()), SID(input.c_str()));
        }
    }
}


void InputModule::SaveContexts(std::string_view contextFilePath) {

    std::string str = mContextsJSON.dump();
    pEngine->Files()->Write(contextFilePath, str);
}


void InputModule::LoadRanges(std::string_view rangesFilePath) {

    // TODO use own JSON class

    std::string jsonTxt(pEngine->Files()->Read(rangesFilePath));
    json::JSON rangesJSON;
    rangesJSON = json::JSON::Load(jsonTxt);
    auto ranges = rangesJSON.ObjectRange();
    for(auto &[name, rangeData] : ranges)
    {
        U64 rangeID = SID(name.c_str());
        mRangeConverter.AddRange(
            rangeID, 
            rangeData[0].ToFloat(),
            rangeData[1].ToFloat(),
            rangeData[2].ToFloat(),
            rangeData[3].ToFloat()
        );
    }
}


void InputModule::PushContext(U64 contextID) {

    mContexts[contextID]->next = pActiveContext;
    pActiveContext = mContexts[contextID];

    auto contextMap = pActiveContext->MappedSignals();
    for(auto &[signal, input] : contextMap)
    {
        mSignalMap[(Size)signal] = input;
    }
}


void InputModule::PopContext() {

    auto contextMap = pActiveContext->MappedSignals();
    for(auto &[signal, input] : contextMap)
    {
        mSignalMap[(Size)signal] = 0;
    }

    auto temp = pActiveContext;
    pActiveContext = pActiveContext->next;
    temp->next = nullptr;
}


void InputModule::SetInputCallback(InputHandler callback) {

    mCallback = callback;
}


bool InputModule::IsKeyPressed(int key) const {

    return glfwGetKey(sWindow, key) == GLFW_PRESS;
}


bool InputModule::IsMouseButtonPressed(int button) const {

    return glfwGetMouseButton(sWindow, button) == GLFW_PRESS;
}


std::pair<double, double> InputModule::GetCursorPos() const {

    return std::make_pair(mouseX, mouseY);
}


void InputModule::KeyListener(GLFWwindow*, int key, int, int action, int) {

    auto inputModule = reinterpret_cast<InputModule*>(glfwGetWindowUserPointer(sWindow));

    Signal signal = GlfwKeyCode(key);
    U64 input = inputModule->mSignalMap[(Size)signal];
    if (input > 0)
    {
        inputModule->mCurrentMappedInput[input].value = action == GLFW_RELEASE ? InputState::RELEASE : InputState::PRESS;
    }
}


void InputModule::MouseListener(GLFWwindow* , int button, int action, int) {

    auto inputModule = reinterpret_cast<InputModule*>(glfwGetWindowUserPointer(sWindow));

    Signal signal = GlfwMouseButtonCode(button);
    U64 input = inputModule->mSignalMap[(Size)signal];
    if (input > 0)
    {
        inputModule->mCurrentMappedInput[input].value = action == GLFW_RELEASE ? InputState::RELEASE : InputState::PRESS;
    }
}


void InputModule::CursorPosListener(GLFWwindow*, double xpos, double ypos) {

    auto inputModule = reinterpret_cast<InputModule*>(glfwGetWindowUserPointer(sWindow));

    U64 mouseMoveX = inputModule->mSignalMap[(Size)Signal::MOUSE_MOVE_X];
    U64 mouseMoveY = inputModule->mSignalMap[(Size)Signal::MOUSE_MOVE_Y];

    if (mouseMoveX > 0)
    {
        inputModule->mCurrentMappedInput[mouseMoveX].value = inputModule->mRangeConverter.Convert(mouseMoveX, xpos - inputModule->mouseX);
    }
    if (mouseMoveY > 0)
    {
        inputModule->mCurrentMappedInput[mouseMoveY].value = inputModule->mRangeConverter.Convert(mouseMoveY, ypos - inputModule->mouseY);
    }

    inputModule->mouseX = xpos;
    inputModule->mouseY = ypos;
}

    
} // Atuin
