
#pragma once


#include "ICVar.h"

#include "utility"
#include <sstream>


namespace Atuin {


template<typename T>
class CVar : public ICVar {

public:

    CVar(const std::string_view name) : ICVar(name), mValue {T()} {}
    CVar(const std::string_view name, T&& value) : ICVar(name), mValue {std::forward<T>(value)} {}
    ~CVar() = default;

    void Set(std::string_view strValue) override {

        std::istringstream iss(strValue.data());
        iss >> mValue;
    }

    void Set(const T &value) {

        mValue = value;
    }

    std::string Get() override {

        std::ostringstream oss;
        oss << mValue;

        return oss.str();
    }

    T Get() const {
        
        return mValue;
    }

private:

    T mValue;
};



} // Atuin