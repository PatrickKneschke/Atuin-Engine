
#include "Files.h"
#include "FileManager.h"


namespace Atuin {
    

FileManager* Files::sFileManager = nullptr;


void Files::MakeDir(std::string_view dirName) const {

    if (sFileManager != nullptr)
    {
        sFileManager->MakeDir((dirName));
    }
}


Array<char> Files::Read(std::string_view fileName, std::ios::openmode mode) const {

    if (sFileManager != nullptr)
    {
        return sFileManager->Read(fileName, mode);   
    }

    return {};
}


void Files::Write(std::string_view fileName, std::string_view buffer, std::ios::openmode mode) const {

    if (sFileManager != nullptr)
    {
        sFileManager->Write(fileName, buffer, mode);   
    }
}


} // Atuin
