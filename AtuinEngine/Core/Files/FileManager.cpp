
#include "FileManager.h"
#include "EngineLoop.h"

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
        std::cout << fileName << " could not be opened!" << '\n';
        // TODO call to error log : "<fileName> could not be opened!";
        return nullptr;
    }

    // get file size 
    file.seekg(0, file.end);
    std::streamsize size = file.tellg();
    file.seekg(0, file.beg);

    char* buffer = new char[size+1];
    if (!file.read(buffer, size))
    {
        std::cout << fileName << " could not be read!" << '\n';
        // TODO call to error log : "<fileName> could not be read!"
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
        std::cout << fileName << " could not be opened!" << '\n';
        // TODO call to error log : "<fileName> could not be opened!";
        return;
    }

    if (!file.write(buffer.data(), buffer.length()))
    {
        std::cout << fileName << " could not be written!" << '\n';
        // TODO call to error log : "<fileName> could not be written!";
        return;
    }    
}


} // Atuin