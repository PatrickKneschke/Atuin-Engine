
#include "Definitions.h"


namespace Atuin {


const std::string ToString(LogLevel level) {

    switch (level) {

        case LogLevel::OFF:
            return "Off";

        case LogLevel::ERROR:
            return "Error";

        case LogLevel::WARNING:
            return "Warning";

        case LogLevel::INFO:
            return "Info";

        case LogLevel::ALL:
            return "All";
                
        default:
            return "Unknown";
    }
}


const std::string ToString(LogChannel channel) {

    switch (channel) {

        case LogChannel::GENERAL:
            return "General";

        case LogChannel::MEMORY:
            return "Memory";

        case LogChannel::FILES:
            return "Files";

        case LogChannel::GRAPHICS:
            return "Graphics";

        case LogChannel::GUI:
            return "GUI";

        case LogChannel::COLLISION:
            return "Collision";

        case LogChannel::AUDIO:
            return "Audio";

        case LogChannel::GAMEPLAY:
            return "Gameplay";

        case LogChannel::ALL:
            return "All";
                
        default:
            return "Unknown";
    }
}


} // Atuin