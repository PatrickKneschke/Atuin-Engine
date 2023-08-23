
#include "InputModule.h"
#include "App.h"
#include "GLFWInput.h"
#include "InputContext.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Files/FileManager.h"
#include "Core/Memory/MemoryManager.h"
#include "Core/Util/StringID.h"

#include "GLFW/glfw3.h"

#include <iostream>


namespace Atuin {


GLFWwindow* InputModule::sWindow = nullptr;

CVar<std::string>* InputModule::pInputContextsFile = ConfigManager::RegisterCVar("Input", "INPUT_CONTEXTS_FILE", std::string("input_contexts.json"));
CVar<std::string>* InputModule::pInputRangesFile   = ConfigManager::RegisterCVar("Input", "INPUT_RANGES_FILE", std::string("input_ranges.json"));


InputModule::InputModule() : 
    mContexts(), 
    pActiveContext {nullptr}, 
    mRangeConverter(), 
    mCurrentMappedInput(), 
    mFiles(), 
    mMemory()
{
    
}


InputModule::~InputModule() {

}


void InputModule::StartUp(GLFWwindow *window) {

    sWindow = window;
    
    glfwSetWindowUserPointer(sWindow, this);

    glfwSetKeyCallback(sWindow, KeyListener);
    glfwSetMouseButtonCallback(sWindow, MouseListener);
    glfwSetCursorPosCallback(sWindow, CursorPosListener);

    LoadContexts( App::sResourceDir->Get() + pInputContextsFile->Get() );
    LoadRanges( App::sResourceDir->Get() + pInputRangesFile->Get() );
}


void InputModule::ShutDown() {

    for (auto [name, context] : mContexts)
    {
        mMemory.Delete(context);
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

    auto content = mFiles.Read( contextFilePath );
    mContextsJSON = Json::Load( std::string_view( content.Data(), content.GetSize() ) );

    auto &contexts = mContextsJSON.GetDict();
    for(auto &[name, contextData] : contexts) 
    {
        U64 nameID = SID(name.c_str());
        mContexts[nameID] = mMemory.New<InputContext>(name); 

        auto inputMap = contextData.GetDict();
        for(auto &[input, signal] : inputMap) 
        {
            mContexts[nameID]->MapSignal( StringToSignal(signal.ToString()), SID(input.c_str()) );
        }
    }
}


void InputModule::SaveContexts(std::string_view contextFilePath) {

    std::string str = mContextsJSON.Print();
    mFiles.Write(contextFilePath, str);
}


void InputModule::LoadRanges(std::string_view rangesFilePath) {

    auto content = mFiles.Read( rangesFilePath );
    Json rangesJSON = Json::Load( std::string_view( content.Data(), content.GetSize() ) );

    auto &ranges = rangesJSON.GetDict();
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
