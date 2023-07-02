
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

    void MakeDir(std::string_view dirName);

    char* Read(std::string_view fileName);
    void Write(std::string_view fileName, std::string_view buffer, std::ios::openmode mode = std::ios::out);

    //TODO add asynch read and write 

private:

    // absolute path to the executable
    std::filesystem::path mRootPath;

    EngineLoop* pEngine;
};

    
} // Atuin
