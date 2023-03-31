
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

    void Read(std::string_view configFile);
    void RegisterCVar(ICVar *cvar);
    void SetCVar(std::string_view name, std::string_view strValue);
    const ICVar* GetCVar(std::string_view name) const ;


private:

    void ProcessConfigFile(const char *content);
    void RemoveComments(std::string *line);
    void ExtractCVar(std::string_view line, std::string *name, std::string *value);

    std::unordered_map<U64, ICVar*> mRegister;
    std::unordered_set<std::string> mCVarNames;

    EngineLoop *pEngine;
};


} // Atuin