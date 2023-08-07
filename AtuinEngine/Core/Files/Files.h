
#pragma once


#include <iostream>
#include <string>


namespace Atuin {


class FileManager;

/* @brief Interface to easily use FileManager throughout the code base.
 */
class Files {

    friend class FileManager;

public:

    Files() : pFileManager {sFileManager} {}

    bool Ready() { return sFileManager != nullptr; }

    void MakeDir(std::string_view dirName) const ;

    char* Read(std::string_view fileName, std::ios::openmode mode = std::ios::in) const ;
    void Write(std::string_view fileName, std::string_view buffer, std::ios::openmode mode = std::ios::out) const ;

private:

    static FileManager* sFileManager;

    FileManager* pFileManager;
};


} // Atuin
