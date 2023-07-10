
#include "Log.h"
#include "Logger.h"

#include "iostream"


namespace Atuin {


Logger* Log::sLogger = nullptr;


void Log::Error(LogChannel channel, std::string_view message, const std::source_location &location) const {

    if (sLogger != nullptr)
    {
        sLogger->Error(channel, message, location);
    }
    else
    {
        throw std::runtime_error( std::string("Error " + ToString(channel) + " : " + message.data() + '\n') );
    }
}


void Log::Warning(LogChannel channel, std::string_view message, const std::source_location &location) const {

    if (sLogger != nullptr)
    {
        sLogger->Warning(channel, message, location);
    }
    else
    {
        std::cerr << "Warning " << ToString(channel) << " : " << message << '\n';
    }
}


void Log::Info(LogChannel channel, std::string_view message, const std::source_location &location) const {

    if (sLogger != nullptr)
    {
        sLogger->Info(channel, message, location);
    }
    else
    {
        std::cerr << "Info " << " , " << ToString(channel) << " : " << message << '\n';
    }
}


void Log::Debug(LogLevel level, LogChannel channel, std::string_view message, const std::source_location &location) const {

    if (sLogger != nullptr)
    {
        sLogger->Debug(level, channel, message, location);
    }
    else
    {
        std::string message( ToString(level) + " , " + ToString(channel) + " : " + message.data() + '\n' );

        if (level == LogLevel::ERROR)
        {
            throw message;
        }
        else
        {
            std::cerr << message;
        }
    }
}



    
} // Atuin
