
#pragma once


#include "Core/Debug/Definitions.h"

#include <source_location>


namespace Atuin {


class Logger;

/* @brief Interface to easily use Logger throughout the code base.
 *        Will default to std::cerr output for Warnings and Info messages or throw in case of Error.
 */
class Log {

    friend class Logger;

public:

    void Error(LogChannel channel, std::string_view message, const std::source_location &location = std::source_location::current()) const;
    void Warning(LogChannel channel, std::string_view message, const std::source_location &location = std::source_location::current()) const;
    void Info(LogChannel channel, std::string_view message, const std::source_location &location = std::source_location::current()) const;
    void Debug(LogLevel level, LogChannel channel, std::string_view message, const std::source_location &location = std::source_location::current()) const;


private:

    static Logger* sLogger;
};

    
} // Atuin
