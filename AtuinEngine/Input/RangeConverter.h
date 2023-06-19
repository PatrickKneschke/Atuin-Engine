
#pragma once


#include "Core/Util/Types.h"

#include "unordered_map"

#include <iostream>


namespace Atuin {


class RangeConverter {

    struct Converter {

        double minIn;
        double maxIn;
        double minOut;
        double maxOut;

        double Convert(double inValue) {
            
            inValue = std::min(std::max(inValue, minIn), maxIn);
            double slope = (maxOut - minOut) / (maxIn - minIn);

            return inValue * slope;
        }
    };

public:

    void AddRange(U64 range, double minIn, double maxIn, double minOut, double maxOut) {

        mRanges[range] = {minIn, maxIn, minOut, maxOut};
    }

    double Convert(U64 range, double value) {

       return mRanges[range].Convert(value);
    }


private:

    std::unordered_map<U64, Converter> mRanges; // TODO replace with custom map
};


} // Atuin