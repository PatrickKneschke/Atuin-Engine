
#pragma once


#include "Core/Util/Types.h"

#include <chrono>


namespace Atuin {


class Clock {

    using Nanoseconds = std::chrono::nanoseconds;
    using HighResClock = std::chrono::high_resolution_clock;
    using TimePoint = std::chrono::time_point<HighResClock>;

public:

    Clock();
    ~Clock() = default;

    void Start();
    void Stop();
    void Reset();

    void Update();

    void TimeScale(float scale);
    float TimeScale() const;

    double DeltaTime() const;
    double ElapsedTime() const;
    double ElapsedUnscaledTime() const;
    U64 ElapsedFrames() const;
    bool IsRunning();


private:

    TimePoint mStartTime;
    TimePoint mCurrTime;
    
    bool mRunning;
    float mTimeScale;

    double mDeltaTime;
    double mElapsedTime;
    double mElapsedUnscaledTime;

    U64 mElapsedFrames;
};


} // Atuin