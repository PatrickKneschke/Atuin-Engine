
#pragma once


#include "Core/Util/Types.h"
#include "CVar.h"

#include <unordered_set>
#include <string>
#include <unordered_map>
#include <utility>

#include <iostream>


namespace Atuin {


class EngineLoop;

class ConfigManager {

    using CVarRegistry = std::unordered_map<U64, ICVar*>;

public:
    
    template<typename T>
    static CVar<T>* RegisterCVar(std::string_view name, T &&value) {

        auto cvar = new CVar<T>(name, std::forward<T>(value));
        auto registry = Registry();
        (*registry)[cvar->Id()] = cvar;

        return cvar;
    }

    ConfigManager(EngineLoop *parent) : pEngine {parent} {}
    ~ConfigManager() = default;

    void Read(std::string_view configFile);
    void SetCVar(std::string_view name, std::string_view strValue);
    const ICVar* GetCVar(std::string_view name) const ;


private:

    static CVarRegistry* Registry();

    void ProcessConfigFile(const char *content);
    void RemoveComments(std::string *line);
    void ExtractCVar(std::string_view line, std::string *name, std::string *value);


    EngineLoop *pEngine;
};


} // Atuin