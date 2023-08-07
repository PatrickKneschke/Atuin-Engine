
#include "FileManager.h"
#include "Files.h"
#include "EngineLoop.h"
#include "Core/Debug/Logger.h"
#include "Core/Util/StringFormat.h"

#include <fstream>

#include <iostream>


namespace Atuin {


FileManager::FileManager() : mRootPath {std::filesystem::current_path()}, mLog(), mJobs() {

    Files::sFileManager = this;
}


void FileManager::MakeDir(std::string_view dirName) {

    std::filesystem::create_directory(dirName);
}


Array<char> FileManager::Read(std::string_view fileName, std::ios::openmode mode) {

    auto filePath = mRootPath / fileName;
    std::ifstream file(filePath, mode);
    if (!file.is_open())
    {
        mLog.Error(LogChannel::FILES, FormatStr("Unable to open file : %s", fileName.data()));

        return {};
    }

    // get file size 
    file.seekg(0, file.end);
    std::streamsize size = file.tellg();
    file.seekg(0, file.beg);

    Array<char> buffer;
    buffer.Resize(size);
    if (!file.read(buffer.Data(), size))
    {
        mLog.Error(LogChannel::FILES, FormatStr("Unable to read file : %s", fileName.data()));
        return {};
    }

    return buffer;
}


void FileManager::Write(std::string_view fileName, std::string_view buffer, std::ios::openmode mode) {

    auto filePath = mRootPath / fileName;
    std::ofstream file(filePath, mode);
    if (!file.is_open())
    {
        mLog.Error(LogChannel::FILES, FormatStr("Unable to open file : %s", fileName.data()));
        return;
    }

    if (!file.write(buffer.data(), buffer.length()))
    {
        mLog.Error(LogChannel::FILES, FormatStr("Unable to write to file : %s", fileName.data()));
        return;
    }    
}


} // Atuin
