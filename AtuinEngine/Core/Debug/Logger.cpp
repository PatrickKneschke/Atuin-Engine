
#include "Logger.h"
#include "EngineLoop.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Files/FileManager.h"
#include "Core/Time/Clock.h"

#include <ctime>
#include <iostream>


namespace Atuin {


CVar<std::string>*  Logger::pLogDir = ConfigManager::RegisterCVar("Logger", "LOG_DIR", std::string("Logs"));
CVar<Size>*         Logger::pMaxMessageChars = ConfigManager::RegisterCVar("Logger", "MAX_MESSAGE_CHARS", 1024UL);
CVar<Size>*         Logger::pBytesToBuffer = ConfigManager::RegisterCVar("Logger", "BYTES_TO_BUFFER", 4096UL);


Logger::~Logger() {

    // in case of crash remaining messenges still get written to log file
    ShutDown();
}


void Logger::StartUp() {

    // TODO assert for LogDir not empty
    // create log directory
    pEngine->Files()->MakeDir(pLogDir->Get());
    
    // assemble log file names
    auto timeStamp = Clock::GetDateTimeStr();
    mFullLog.fileName = pLogDir->Get() + "/full_" + timeStamp + ".log";
    mChannelsLog.fileName = pLogDir->Get() + "/channels_" + timeStamp + ".log";
}


void Logger::ShutDown() {

    std::string content;

    mFullLog.stream.flush();
    content = mFullLog.stream.str(); 
    // TODO use async write instead
    pEngine->Files()->Write(mFullLog.fileName, content.c_str(), content.length(), std::ios::app);

    mFullLog.stream.str("");
    mFullLog.stream.clear();
    mFullLog.stream.seekp(0);

    mChannelsLog.stream.flush();
    content = mChannelsLog.stream.str(); 
    // TODO use async write instead
    pEngine->Files()->Write(mChannelsLog.fileName, content.c_str(), content.length(), std::ios::app);

    mChannelsLog.stream.str("");
    mChannelsLog.stream.clear();
    mChannelsLog.stream.seekp(0);
}


void Logger::Error(LogChannel channel, std::string_view message, const std::source_location location) {

    Debug(LogLevel::ERROR, channel, message, location);
}


void Logger::Warning(LogChannel channel, std::string_view message, const std::source_location location) {

    Debug(LogLevel::WARNING, channel, message, location);
}


void Logger::Info(LogChannel channel, std::string_view message, const std::source_location location) {

    Debug(LogLevel::INFO, channel, message, location);
}


void Logger::Debug(LogLevel level, LogChannel channel, std::string_view message, const std::source_location location) {

    std::string file = std::filesystem::path(location.file_name()).filename();
    U32 line = location.line();
    DebugPrint(file, line, level, channel, message);
}


 void Logger::DebugPrint(std::string_view file, U32 line, LogLevel level, LogChannel channel, std::string_view message) {

    std::ostringstream oss;
    oss << file << " [" << line << "] , " << ToString(level) << " ,  " << ToString(channel) << "  :  " << message << '\n';

    if (mLevelMask.test((Size)level-1))
    {
        // TODO do engine console output instead
        std::cout << oss.str();
    }

    // write to unfiltered log file
    auto buffer = oss.str();
    WriteBufferToFile(mFullLog, buffer.c_str());

    // write to filtered log
    if (mChannelMask.test((Size)channel))
    {
        WriteBufferToFile(mChannelsLog, buffer.c_str());
    }
 }


 void Logger::WriteBufferToFile(LogWriter &writer, const char *buffer) {

    writer.stream << buffer;
    writer.stream.seekp(0, std::ios::end);
    if ( static_cast<Size>(writer.stream.tellp()) >= pBytesToBuffer->Get() )
    {
        writer.stream.flush();

        auto content = writer.stream.str(); 
        // TODO use async write instead
        pEngine->Files()->Write(writer.fileName, content.c_str(), content.length(), std::ios::app);

        writer.stream.str("");
        writer.stream.clear();
        writer.stream.seekp(0);
    }        
 }


} // Atuin