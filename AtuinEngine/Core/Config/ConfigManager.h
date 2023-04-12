
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

    struct CVarBlock {

        std::string name;
        U64 id;
        std::unordered_map<U64, ICVar*> cvars;
    } ;

    using CVarRegistry = std::unordered_map<U64, CVarBlock>;

public:
    
    template<typename T>
    static CVar<T>* RegisterCVar(std::string_view block, std::string_view name, T &&value);

    ConfigManager(EngineLoop *parent) : pEngine {parent} {}
    ~ConfigManager() = default;

    void Read(std::string_view configFile);
    void Save() const;

    void SetCVar(std::string_view blockName, std::string_view cvarName, std::string_view strValue);
    const ICVar* GetCVar(std::string_view blockName, std::string_view cvarName) const ;


private:

    static CVarRegistry* GetRegistry();

    void ProcessConfigFile(const char *content);
    void RemoveComments(std::string *line);
    void ExtractBlock(std::string_view line, std::string *block);
    void ExtractCVar(std::string_view line, std::string *name, std::string *value);

    std::string mConfigFile;

    EngineLoop* pEngine;
};


template<typename T>
CVar<T>* ConfigManager::RegisterCVar(std::string_view blockName, std::string_view cvarName, T &&value) {

    auto registry = GetRegistry();
    auto newCVar = new CVar<T>(cvarName, std::forward<T>(value));

    U64 blockId = SID(blockName.data());
    if (registry->find(blockId) == registry->end())
    {
        registry->insert( {blockId, CVarBlock{blockName.data(), blockId, {}}} );
    }
    registry->at(blockId).cvars.insert( {newCVar->Id(), newCVar} );

    return newCVar;
}


} // Atuin