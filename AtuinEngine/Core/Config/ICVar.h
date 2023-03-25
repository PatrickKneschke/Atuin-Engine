
#pragma once


#include "Core/Util/StringID.h"
#include "Core/Util/Types.h"

#include <string>


namespace Atuin {


class ICVar {

public:

    ICVar(const std::string_view name) : mName {name}, mID {SID(name.data())} {}
    virtual ~ICVar() = 0;

    virtual void Set(const std::string_view strValue) = 0;

protected:

    std::string mName;
    U64 mID;
};

    
} // Atuin

