
#pragma once


#include "Core/Util/Types.h"

#include <unordered_set>
#include <string>
#include <unordered_map>


namespace Atuin {


class EngineLoop;
class ICVar;

class ConfigManager {

public:
    ConfigManager(EngineLoop *parent) : pEngine {parent} {}
    ~ConfigManager() = default;

    void StartUp();
    void RegisterCVar(std::string_view name, std::string_view strValue);
    void SetCVarValue(std::string_view name, std::string_view strValue);


private:

    std::unordered_map<U64, ICVar*> mRegister;
    std::unordered_set<std::string> mCVarNames;

    EngineLoop *pEngine;
};


} // Atuin