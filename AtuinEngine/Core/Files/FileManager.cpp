
#include "FileManager.h"
#include "EngineLoop.h"
#include "Core/Debug/Logger.h"
#include "Core/Util/StringFormat.h"

#include <fstream>

#include <iostream>


namespace Atuin {


void FileManager::MakeDir(std::string_view dirName) {

    std::filesystem::create_directory(dirName);
}


char* FileManager::Read(std::string_view fileName) {

    auto filePath = mRootPath / fileName;
    std::ifstream file(filePath);
    if (!file.is_open())
    {
        pEngine->Log()->Error(LogChannel::FILES, FormatStr("Unable to open file : %s", fileName.data()));

        return nullptr;
    }

    // get file size 
    file.seekg(0, file.end);
    std::streamsize size = file.tellg();
    file.seekg(0, file.beg);

    char* buffer = new char[size+1];
    if (!file.read(buffer, size))
    {
        pEngine->Log()->Error(LogChannel::FILES, FormatStr("Unable to read file : %s", fileName.data()));
        return nullptr;
    }
    buffer[size] = '\0';

    return buffer;
}


void FileManager::Write(std::string_view fileName, std::string_view buffer, std::ios::openmode mode) {

    auto filePath = mRootPath / fileName;
    std::ofstream file(filePath, mode);
    if (!file.is_open())
    {
        pEngine->Log()->Error(LogChannel::FILES, FormatStr("Unable to open file : %s", fileName.data()));
        return;
    }

    if (!file.write(buffer.data(), buffer.length()))
    {
        pEngine->Log()->Error(LogChannel::FILES, FormatStr("Unable to write to file : %s", fileName.data()));
        return;
    }    
}


} // Atuin
