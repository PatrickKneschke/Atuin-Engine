
#include "Clock.h"


namespace Atuin {


Clock::Clock() {

    Reset();
}


void Clock::Start() {

    mRunning = true;
}


void Clock::Stop() {

    mRunning = false;
}


void Clock::Reset() {

    mStartTime = HighResClock::now();
    mCurrTime = mStartTime;
    mRunning = true;
    mTimeScale = 0.f;
    mDeltaTime = 0.0;
    mElapsedTime = 0.0;
    mElapsedUnscaledTime = 0.0;
    mElapsedFrames = 0;
}


void Clock::Update() {

    ++mElapsedFrames;
    mDeltaTime = 0.0;
    if (mRunning)
    {
        auto now = HighResClock::now();
        mDeltaTime = mTimeScale * 1e-9 * std::chrono::duration_cast<Nanoseconds>(now - mCurrTime).count();
        mElapsedUnscaledTime = 1e-9 * std::chrono::duration_cast<Nanoseconds>(now - mStartTime).count();
        mElapsedTime += mDeltaTime;
        mCurrTime = now;
    }  
}


float Clock::TimeScale() const {

    return mTimeScale;
}


void Clock::TimeScale(float scale) {

    mTimeScale = scale;
}


double Clock::DeltaTime() const {

    return mDeltaTime;
}


double Clock::ElapsedTime() const {
    
    return mElapsedTime;
}


double Clock::ElapsedUnscaledTime() const {

    return mElapsedUnscaledTime;
}


uint64_t Clock::ElapsedFrames() const {

    return mElapsedFrames;
}


bool Clock::IsRunning() {

    return mRunning;
}

    
} // Atuin