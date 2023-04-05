
#pragma once


#include "Core/Util/Types.h"

#include <filesystem>
#include <string>


namespace Atuin
{


class EngineLoop;

class FileManager {

public:
    FileManager(EngineLoop *parent) : mRootPath {std::filesystem::current_path()}, pEngine {parent} {}
    ~FileManager() = default;

    char* Read(std::string_view fileName);
    void Write(std::string_view fileName, const char *buffer, Size size);


private:

    // absolute path to the executable
    std::filesystem::path mRootPath;

    EngineLoop *pEngine;
};

    
} // Atuin
