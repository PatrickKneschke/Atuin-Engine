
#include "ConfigManager.h"
#include "ICVar.h"
#include "Core/Files/FileManager.h"
#include "Core/Util/StringID.h"
#include "EngineLoop.h"

#include <iostream>


namespace Atuin {


void ConfigManager::StartUp() {

    const char *content = pEngine->Files()->Read("AtuinEngine/config.ini");

    std::cout << content << '\n';
}


void ConfigManager::RegisterCVar(std::string_view name, std::string_view strValue) {

    mRegister[SID(name.data())]->Set(strValue);
}


void ConfigManager::SetCVarValue(std::string_view name, std::string_view strValue) {
 
    if(mCVarNames.find(name.data()) == mCVarNames.end()) {

        // TODO : call to error log : "<name> is not a registered CVar!"
        return;
    }

    mRegister[SID(name.data())]->Set(strValue);
}


} // Atuin