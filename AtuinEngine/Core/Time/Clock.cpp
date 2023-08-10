
#include "Clock.h"


namespace Atuin {



std::string Clock::GetDateTimeStr() {

    auto now = std::time(nullptr);
    char buffer[sizeof("YYYY-MM-DD_HH:MM:SS")];    
    auto dateTimeStr = std::string(buffer, buffer + std::strftime(buffer, sizeof(buffer),"%F_%T", std::gmtime(&now)));

    return dateTimeStr;
}

std::string Clock::GetDateStr() {

    return GetDateTimeStr().substr(0, 10);
}

std::string Clock::GetTimeStr() {

    return GetDateTimeStr().substr(11);
}


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
        mDeltaTime = mTimeScale * (double)std::chrono::duration_cast<Nanoseconds>(now - mCurrTime).count() * 1e-9;
        mElapsedUnscaledTime = (double)std::chrono::duration_cast<Nanoseconds>(now - mStartTime).count() * 1e-9;
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