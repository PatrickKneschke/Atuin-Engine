
#pragma once


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


private:

    // absolute path to the executable
    std::filesystem::path mRootPath;

    EngineLoop *pEngine;
};

    
} // Atuin
