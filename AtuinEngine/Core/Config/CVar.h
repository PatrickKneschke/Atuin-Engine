
#pragma once


#include "ICVar.h"

#include "utility"
#include <sstream>


namespace Atuin {


template<typename T>
class CVar : public ICVar {

public:

    CVar(const std::string_view name) : ICVar(name), mValue {T()} {}
    CVar(const std::string_view name, const T &value) : ICVar(name), mValue {value} {}
    CVar(const std::string_view name, T&& value) : ICVar(name), mValue {std::forward<T>(value)} {}
    ~CVar() = default;

    // TODO only works for types that support istream::operator>>, also need vector CVars
    void SetValueStr(std::string_view valueStr) override {

        mValueStr = valueStr;

        std::istringstream iss(valueStr.data());
        iss >> mValue;
    }

    std::string GetValueStr() override {

        return mValueStr;
    }

    void Set(const T &value) {

        mValue = value;
    }

    T Get() const {
        
        return mValue;
    }

private:

    T mValue;
    std::string mValueStr;
};



} // Atuin