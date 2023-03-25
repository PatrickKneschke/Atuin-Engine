
#include "ConfigManager.h"
#include "ICVar.h"
#include "Core/Util/StringID.h"


namespace Atuin {


void ConfigManager::StartUp() {

    const char *content = ReadConfig("config.txt");
}


void ConfigManager::SetCVarValue(std::string_view name, std::string_view strValue) {

    mRegister[SID(name.data())]->Set(strValue);
}


const char* ConfigManager::ReadConfig(std::string_view fileName) {

    // call to file system TODO
    return nullptr;
}


} // Atuin