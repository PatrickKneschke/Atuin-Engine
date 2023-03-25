
#pragma once


#include "Core/Util/Types.h"

#include <string>
#include <vector>
#include <unordered_map>


namespace Atuin {


class ICVar;

class ConfigManager {

public:
    ConfigManager() = default;
    ~ConfigManager() = default;

    void StartUp();
    void SetCVarValue(std::string_view name, std::string_view strValue);

private:

    const char* ReadConfig(std::string_view fileName);

    std::unordered_map<U64, ICVar*> mRegister;
    std::vector<std::string> mCVarNames;
};


} // Atuin