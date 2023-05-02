
#pragma once


#include "Types.h"

#include <stdio.h>
#include <string>


namespace Atuin {


template <typename... Args>
std::string FormatStr(std::string_view format, Args&&... args) {

    Size size = std::snprintf(nullptr, 0, format.data(), args...) + 1;

    char *buffer = new char[size];
    std::snprintf(buffer, size, format.data(), args...);

    return std::string(buffer, buffer + size - 1);
}


} // Atuin