#include "time.h"
#include "window.h"

double EngineTime() {
    return GetTime();
}

void SetEngineTime(double time) {
    SetTime(time);
}

unsigned long long GetTimerValue() {
    return ::GetTimerValue();
}

unsigned long long GetTimerFrequency() {
    return ::GetTimerFrequency();
}