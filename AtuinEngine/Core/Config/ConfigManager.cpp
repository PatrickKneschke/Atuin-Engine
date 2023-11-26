
#include "ConfigManager.h"
#include "ICVar.h"
#include "Core/Util/Types.h"
#include "Core/Util/StringID.h"
#include "Core/Util/StringFormat.h"

#include <iomanip>
#include <string>


namespace Atuin {


ConfigManager::CVarRegistry* ConfigManager::GetRegistry() {

    static CVarRegistry registry;
    return &registry;
}


void ConfigManager::Read(std::string_view configFile) {

    mConfigFile = configFile;

    Array<char> content;
     mFiles.Read(configFile, content);
    ProcessConfigFile( content.Data(), content.GetSize() );
}


void ConfigManager::Save() const {

    std::ostringstream oss(std::ios::ate);
    for(auto &[blockId, block] : *GetRegistry())
    {
        oss << "\n[" << block.name << "]\n";
        for (auto &[cvarId, cvar] : block.cvars)
        {
            oss << std::setw(20) << std::left << cvar->Name() << " = " << cvar->GetValueStr() << '\n';
        }
    }

    mFiles.Write(mConfigFile, oss.str());
}


void ConfigManager::SetCVar(std::string_view blockName, std::string_view cvarName, std::string_view strValue) {
 
    auto registry = GetRegistry();
    U64 blockId = SID(blockName.data());
    U64 cvarId = SID(cvarName.data());
    if (registry->find(blockId) == registry->end())
    {
        mLog.Warning(LogChannel::GENERAL, FormatStr("%s is not a known block of CVars!", blockName.data()));
        return;
    }

    if (registry->at(blockId).cvars.find(cvarId) == registry->at(blockId).cvars.end())
    {
        mLog.Warning(LogChannel::GENERAL, FormatStr("%s is not registered CVar!", cvarName.data()));
        return;
    }

    registry->at(blockId).cvars[cvarId]->SetValueStr(strValue);
}


const ICVar* ConfigManager::GetCVar(std::string_view blockName, std::string_view cvarName) const {
 
    auto registry = GetRegistry();
    U64 blockId = SID(blockName.data());
    U64 cvarId = SID(cvarName.data());
    if (registry->find(blockId) == registry->end())
    {
        mLog.Error(LogChannel::GENERAL, FormatStr("%s is not a known block of CVars!", blockName.data()));
        return nullptr;
    }
    
    if (registry->at(blockId).cvars.find(cvarId) == registry->at(blockId).cvars.end())
    {
        mLog.Error(LogChannel::GENERAL, FormatStr("%s is not registered CVar!", cvarName.data()));
        return nullptr;
    }

    return registry->at(blockId).cvars[cvarId];
}


void ConfigManager::ProcessConfigFile(const char *content, Size size) {  

    std::string_view lines(content, size);
    Size startPos = 0, endPos = 0, maxPos = lines.length();
    std::string currBlock;
    while (startPos < maxPos)
    {
        endPos = std::min(maxPos, lines.find('\n', startPos));
        if (endPos > startPos)
        {
            std::string nextLine(lines.substr(startPos, endPos - startPos));

            RemoveComments(&nextLine);
            ExtractBlock(nextLine, &currBlock);

            std::string name, value;
            ExtractCVar(nextLine, &name, &value);

            if (name.length() > 0 && value.length() > 0)
            {
                SetCVar(currBlock, name, value);
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


void ConfigManager::ExtractBlock(std::string_view line, std::string *block) {

    Size start = line.find_first_of('[', 0);
    Size end = line.find_first_of(']', 0);
    if (start == std::string::npos || end == std::string::npos)
    {
        return;
    }

    *block = line.substr(start+1, end - start - 1);
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
    // stop if no seperator was found
    if (end == std::string::npos)
    {
        return;
    }
    *name = line.substr(start, end-start);

    // remove trailing spaces
    Size tail = name->find_first_of(" \t");
    if (tail != std::string::npos)
    {
        (*name).erase(name->find_first_of(" \t"));
    }

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