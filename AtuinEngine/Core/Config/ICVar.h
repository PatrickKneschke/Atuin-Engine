
#pragma once


#include "Core/Util/StringID.h"
#include "Core/Util/Types.h"

#include <string>


namespace Atuin {


class ICVar {

public:

    ICVar(const std::string_view name) : mName {name}, mId {SID(name.data())} {}
    ~ICVar() = default;

    virtual void Set(const std::string_view strValue) = 0;
    std::string Name() { return mName; }
    U64 Id() { return mId; };

protected:

    std::string mName;
    U64 mId;
};

    
} // Atuin

