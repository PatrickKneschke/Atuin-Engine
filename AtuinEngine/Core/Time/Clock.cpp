
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
    mRunning = false;
    mTimeScale = 1.0f;
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
        mDeltaTime = mTimeScale * std::chrono::duration_cast<Nanoseconds>(now - mCurrTime).count() * 1e-9;
        mElapsedUnscaledTime = std::chrono::duration_cast<Nanoseconds>(now - mStartTime).count() * 1e-9;
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


U64 Clock::ElapsedFrames() const {

    return mElapsedFrames;
}


bool Clock::IsRunning() {

    return mRunning;
}

    
} // Atuin