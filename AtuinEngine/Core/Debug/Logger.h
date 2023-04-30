
#pragma once


#include "Core/Debug/Definitions.h"
#include "Core/Config/CVar.h"
#include "Core/Util/Types.h"

#include <bitset>
#include <source_location>
#include <string>


namespace Atuin {


class EngineLoop;

class Logger {


    struct LogWriter {

        std::ostringstream stream;
        std::string fileName;
    };


public:

    Logger(EngineLoop *engine): mLevelMask {~0UL}, mChannelMask {~0UL},  pEngine {engine} {}
    ~Logger();
    

    void StartUp();
    void ShutDown();

    void Error(LogChannel channel, std::string_view message, const std::source_location location = std::source_location::current());
    void Warning(LogChannel channel, std::string_view message, const std::source_location location = std::source_location::current());
    void Info(LogChannel channel, std::string_view message, const std::source_location location = std::source_location::current());
    void Debug(LogLevel level, LogChannel channel, std::string_view message, const std::source_location location = std::source_location::current());


private:

    void DebugPrint(std::string_view file, U32 line, LogLevel level, LogChannel channel, std::string_view message);
    void WriteBufferToFile(LogWriter &writer, const char *buffer);

    // config variables
    static CVar<std::string>*   pLogDir;
    static CVar<Size>*          pMaxMessageChars;   // TODO either use or remove
    static CVar<Size>*          pBytesToBuffer;
    
    // log level mask
    std::bitset<(Size)LogLevel::ALL> mLevelMask;
    // log channel mask
    std::bitset<(Size)LogChannel::ALL> mChannelMask;

    // writers for full and channel-filtered log files
    LogWriter mFullLog;
    LogWriter mChannelsLog;

    EngineLoop* pEngine;
};


} // Atuin