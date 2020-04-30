#include "core.h"
#include "timer.h"
#include <chrono>
#include <thread>

NLUA_BEGIN

struct TimeCounterPrivate {
    chrono::high_resolution_clock::time_point t0;
};

TimeCounter::TimeCounter() {
    NLUA_CLASS_CONSTRUCT();
}

TimeCounter::~TimeCounter() {
    NLUA_CLASS_DESTORY();
}

void TimeCounter::start() {
    d_ptr->t0 = chrono::high_resolution_clock::now();
}

double TimeCounter::seconds(bool reset) {
    auto t1 = chrono::high_resolution_clock::now();
    auto dur = t1 - d_ptr->t0;

    if (reset) {
        d_ptr->t0 = t1;
    }

    return chrono::duration_cast<chrono::microseconds>(dur).count() * 0.000001;
}

void Time::Sleep(double seconds) {
    this_thread::sleep_for(chrono::microseconds(long(seconds * 1e6)));
}

NLUA_END
