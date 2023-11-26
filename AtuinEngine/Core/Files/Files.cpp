
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


void Files::Read(std::string_view fileName, Array<char> &buffer, std::ios::openmode mode) const {

    if ( Ready() )
    {
        sFileManager->Read(fileName, buffer, mode);
    }
}


void Files::Write(std::string_view fileName, std::string_view buffer, std::ios::openmode mode) const {

    if (sFileManager != nullptr)
    {
        sFileManager->Write(fileName, buffer, mode);   
    }
}


void Files::ReadAsync(void *data) const {

    auto readData = (AsyncReadData*)data;

    return Read( readData->fileName, readData->buffer, readData->mode);
}


void Files::WriteAsync(void *data) const {

    auto writeData = (AsyncWriteData*)data;

    Write( writeData->fileName, writeData->buffer, writeData->mode);
}


} // Atuin
