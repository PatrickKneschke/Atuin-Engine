
#pragma once


#include "Core/Util/Types.h"

#include <string>


namespace Atuin {



enum class LogLevel : U8 {

    OFF = 0,
    ERROR,
    WARNING,
    INFO,
    ALL
};

const std::string ToString(LogLevel level);


enum class LogChannel : U8 {

    GENERAL = 0,
    MEMORY,
    FILES,
    GRAPHICS,
    GUI,
    COLLISION,
    AUDIO,
    GAMEPLAY,
    ALL
};

const std::string ToString(LogChannel channel);


} // Atuin