
#include "Logger.h"
#include "EngineLoop.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Files/FileManager.h"
#include "Core/Time/Clock.h"

#include "assert.h"
#include <ctime>


namespace Atuin {


CVar<std::string>*  Logger::pLogDir = ConfigManager::RegisterCVar("Logger", "LOG_DIR", std::string("Logs"));
CVar<Size>*         Logger::pMaxMessageChars = ConfigManager::RegisterCVar("Logger", "MAX_MESSAGE_CHARS", 1024UL);
CVar<Size>*         Logger::pBytesToBuffer = ConfigManager::RegisterCVar("Logger", "BYTES_TO_BUFFER", 4096UL);


Logger::~Logger() {

    // in case of crash remaining messenges still get written to log file
    ShutDown();
}


void Logger::StartUp() {

    assert(pLogDir->Get().length() > 0);

    // create log directory
    pEngine->Files()->MakeDir(pLogDir->Get());
    
    // assemble log file names
    auto timeStamp = Clock::GetDateTimeStr();
    mFullLog.fileName = pLogDir->Get() + "/full_" + timeStamp + ".log";
    mChannelsLog.fileName = pLogDir->Get() + "/channels_" + timeStamp + ".log";

    mLevelMask.flip((Size)LogLevel::WARNING);
    mChannelMask.flip((Size)LogChannel::GENERAL);
}


void Logger::ShutDown() {

    mFullLog.stream.flush();
    // TODO use async write instead
    pEngine->Files()->Write(mFullLog.fileName, mFullLog.stream.str(), std::ios::app);

    mFullLog.stream.str("");
    mFullLog.stream.clear();
    mFullLog.stream.seekp(0);

    mChannelsLog.stream.flush();
    // TODO use async write instead
    pEngine->Files()->Write(mChannelsLog.fileName, mChannelsLog.stream.str(), std::ios::app);

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

    if (mLevelMask.test((Size)level))
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
    // write to file if file manager is created and enough characters have been buffered
    if ( static_cast<Size>(writer.stream.tellp()) >= pBytesToBuffer->Get() && pEngine->Files() != nullptr )
    {
        writer.stream.flush();
 
        // TODO use async write instead
        pEngine->Files()->Write(writer.fileName, writer.stream.str(), std::ios::app);

        writer.stream.str("");
        writer.stream.clear();
        writer.stream.seekp(0);
    }        
 }


} // Atuin