
#pragma once


#include "Core/Config/CVar.h"
#include "Core/Util/Types.h"

#include <source_location>
#include <string>


namespace Atuin {


class EngineLoop;

class Logger {

public:

    Logger(EngineLoop *engine) : pEngine {engine} {}
    ~Logger() = default;

    void Error(std::string_view message, const std::source_location location = std::source_location::current());
    void Warning();
    void Info();


private:

    // config variables
    static CVar<std::string>*   pLogDir;
    static CVar<Size>*          pMaxMessageChars;
    static CVar<Size>*          pBytesToBuffer;

    EngineLoop* pEngine;
};


} // Atuin