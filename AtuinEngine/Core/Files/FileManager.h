
#pragma once


#include "Core/Util/Types.h"
#include "Core/Debug/Log.h"
#include "Core/Jobs/Jobs.h"

#include <filesystem>
#include <string>


namespace Atuin {


class FileManager {

public:
    FileManager();
    ~FileManager() = default;

    void MakeDir(std::string_view dirName);

    char* Read(std::string_view fileName, std::ios::openmode mode = std::ios::in);
    void Write(std::string_view fileName, std::string_view buffer, std::ios::openmode mode = std::ios::out);

    //TODO add asynch read and write 

private:

    // absolute path to the executable
    std::filesystem::path mRootPath;

    Log mLog;
    Jobs mJobs;
};

    
} // Atuin
