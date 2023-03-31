
#include "ConfigManager.h"
#include "ICVar.h"
#include "Core/Files/FileManager.h"
#include "Core/Util/Types.h"
#include "Core/Util/StringID.h"
#include "EngineLoop.h"

#include <iostream>


namespace Atuin {


void ConfigManager::StartUp() {

    const char *content = pEngine->Files()->Read("AtuinEngine/config.ini");
    ProcessConfigFile(content);
}


void ConfigManager::RegisterCVar(ICVar *cvar) {


    mRegister[cvar->Id()] = cvar;
    mCVarNames.insert(cvar->Name());
}


void ConfigManager::SetCVar(std::string_view name, std::string_view strValue) {
 
    U64 id = SID(name.data());
    if (mRegister.find(id) == mRegister.end())
    {
        // TODO : call to error log : "<name> is not a registered CVar!"
        return;
    }

    mRegister[id]->Set(strValue);
}


const ICVar* ConfigManager::GetCVar(std::string_view name) const {
 
    U64 id = SID(name.data());
    if (mRegister.find(id) == mRegister.end())
    {
        // TODO : call to error log : "<name> is not a registered CVar!"
        return nullptr;
    }

    return mRegister.at(id);
}


void ConfigManager::ProcessConfigFile(const char *content) {

    std::string_view lines(content);
    Size startPos = 0, endPos = 0, maxPos = lines.length();
    while (startPos < maxPos)
    {
        endPos = std::min(maxPos, lines.find('\n', startPos));
        if (endPos > startPos)
        {
            std::string nextLine = lines.substr(startPos, endPos - startPos).data();
            RemoveComments(&nextLine);
            std::string name, value;
            ExtractCVar(nextLine, &name, &value);

            if (name.length() > 0 && value.length() > 0)
            {
                SetCVar(name, value);
            }
        }

        startPos = endPos + 1;
    }
}


void ConfigManager::RemoveComments(std::string *line) {

    Size pos = line->find('#');
    if (pos != std::string::npos)
    {
        line->erase(pos);
    }
}


void ConfigManager::ExtractCVar(std::string_view line, std::string *name, std::string *value) {
    
    // skip leading spaces and tabs
    Size start = line.find_first_not_of(" \t", 0);

    // stop if line is empty or has no CVar name
    if (start == std::string::npos || line[start] == '=')
    {
        return;
    }

    // extract CVar name
    Size end = line.find_first_of('=', start);
    *name = line.substr(start, end-start);
    (*name).erase(name->find_first_of(" \t"));

    // extract CVar value
    start = line.find_first_not_of(" \t", end + 1);
    // stop if line has no CVar value
    if (start == std::string::npos)
    {
        return;
    }
    
    end = line.find_first_of(" \t\n", start);
    *value = line.substr(start, end - start);
}



} // Atuin