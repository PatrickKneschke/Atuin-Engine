
#pragma once


#include "Core/Util/Types.h"
#include "Core/Util/StringID.h"
#include "Core/DataStructures/Array.h"
#include "Input/InputSignals.h"

#include <string>


namespace Atuin {


class InputContext {

public:

    InputContext(std::string_view name, bool blocking = true, InputContext *next_ = nullptr) : 
        next {next_}, 
        mName {name.data()}, 
        mNameID {SID(name.data())}, 
        mSignalMap {}, 
        mBlocking {blocking}
    {
        // mEvents.Resize(static_cast<Size>(Signal::COUNT), 0);
    }

    void MapSignal(Signal signal, U64 inputID) {
        
        mSignalMap[signal] = inputID;
    }

    std::string Name() const { return mName; }
    U64         NameID() const { return mNameID; }
    bool IsBlocking() const { return mBlocking; }
    const std::unordered_map<Signal, U64>& MappedSignals() const { return mSignalMap; } 


    InputContext* next;


private:

    std::string mName;
    U64 mNameID;
    // Array<U64>    mEvents;
    std::unordered_map<Signal, U64> mSignalMap; // TODO replace with custom map
    bool mBlocking;
};

    
} // Atuin
