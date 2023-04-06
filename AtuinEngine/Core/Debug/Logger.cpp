
#include "Logger.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Files/FileManager.h"

#include <iostream>


namespace Atuin {


CVar<std::string>*   Logger::pLogDir = ConfigManager::RegisterCVar("Logger", "LOG_DIR", std::string("Logs"));
CVar<Size>*          Logger::pMaxMessageChars = ConfigManager::RegisterCVar("Logger", "MAX_MESSAGE_CHARS", 1024UL);
CVar<Size>*          Logger::pBytesToBuffer  = ConfigManager::RegisterCVar("Logger", "BYTES_TO_BUFFER", 4096UL);


void Logger::Error(std::string_view message, const std::source_location location) {

    std::cout << location.file_name() << " : " << message << '\n';
}


void Logger::Warning() {


}


void Logger::Info() {


}


} // Atuin