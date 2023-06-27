
#pragma once


#include "Core/Util/Types.h"
#include "Core/Util/StringID.h"
#include "Core/DataStructures/Map.h"
#include "Input/InputSignals.h"

#include <string>


namespace Atuin {


class InputContext {

public:

    InputContext(std::string_view name, bool blocking = true, InputContext *next_ = nullptr) : 
        next {next_}, 
        mName {name.data()}, 
        mNameID {SID(name.data())}, 
        mSignalMap(), 
        mBlocking {blocking}
    {

    }

    void MapSignal(Signal signal, U64 inputID) {
        
        mSignalMap[signal] = inputID;
    }

    std::string Name() const { return mName; }
    U64         NameID() const { return mNameID; }
    bool IsBlocking() const { return mBlocking; }
    const Map<Signal, U64>& MappedSignals() const { return mSignalMap; } 


    InputContext* next;


private:

    std::string mName;
    U64 mNameID;
    Map<Signal, U64> mSignalMap; // TODO replace with custom map
    bool mBlocking;
};

    
} // Atuin
