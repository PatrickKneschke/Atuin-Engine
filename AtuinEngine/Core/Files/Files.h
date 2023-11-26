
#pragma once


#include "Core/DataStructures/Array.h"

#include <iostream>
#include <string>


namespace Atuin {


class FileManager;

/* @brief Interface to easily use FileManager throughout the code base.
 */
class Files {

    friend class FileManager;

public:

    struct AsyncReadData {

        std::string fileName;
        Array<char> buffer;
        std::ios::openmode mode = std::ios::in;
    };

    struct AsyncWriteData {

        std::string fileName;
        std::string buffer;
        std::ios::openmode mode = std::ios::out;
    };


    Files() : pFileManager {sFileManager} {}

    bool Ready() const { return sFileManager != nullptr; }

    void MakeDir(std::string_view dirName) const ;

    void Read(std::string_view fileName, Array<char> &buffer, std::ios::openmode mode = std::ios::in) const ;
    void Write(std::string_view fileName, std::string_view buffer, std::ios::openmode mode = std::ios::out) const ;
 
    void ReadAsync(void *data) const;
    void WriteAsync(void *data) const;

private:

    static FileManager* sFileManager;

    FileManager* pFileManager;
};


} // Atuin
