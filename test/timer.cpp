#include "nlua++.h"
#include "timer.h"
#include <chrono>
#include <thread>
#include <map>
#include <mutex>

NLUA_BEGIN

struct TimeCounterPrivate
{
    chrono::high_resolution_clock::time_point t0;
};

TimeCounter::TimeCounter()
{
    NLUA_CLASS_CONSTRUCT();
}

TimeCounter::~TimeCounter()
{
    NLUA_CLASS_DESTORY();
}

void TimeCounter::start()
{
    d_ptr->t0 = chrono::high_resolution_clock::now();
}

double TimeCounter::seconds(bool reset)
{
    auto t1 = chrono::high_resolution_clock::now();
    auto dur = t1 - d_ptr->t0;

    if (reset)
    {
        d_ptr->t0 = t1;
    }

    return chrono::duration_cast<chrono::microseconds>(dur).count() * 0.000001;
}

void Time::Sleep(double seconds)
{
    this_thread::sleep_for(chrono::microseconds(long(seconds * 1e6)));
}

struct CoTimerItem
{
    CoTimers::timer_t id;
    double seconds;
    double left;
    int repeat;
    CoTimers::tick_t tick;
};

struct CoTimersPrivate
{

    CoTimersPrivate() : thd(new thread(Tick, this))
    {
        thd->detach();
    }

    ~CoTimersPrivate()
    {
        running = false;
        thd->join();
    }

    static void Tick(CoTimersPrivate *self)
    {
        while (self->running)
        {
            NLUA_AUTOGUARD(self->mtx)
            Time::Sleep(self->interval);
            bool needclean = false;

            for (auto &e : self->timers)
            {
                auto &item = *e.second;
                item.left -= self->interval;
                if (item.left <= 0)
                {
                    if (item.repeat != -1)
                    {
                        if (--item.repeat == 0)
                            needclean = true;
                    }
                    // 先做为同步调用
                    item.tick();
                    item.left = item.seconds;
                }
            }

            // 移除失效的
            if (needclean)
            {
                for (auto i = self->timers.begin(); i != self->timers.end();)
                {
                    if (i->second->repeat <= 0) {
                        i = self->timers.erase(i);
                    }
                    else {
                        ++i;
                    }
                }
            }
        }
    }

    double interval = 0;
    CoTimers::timer_t counter = 0;
    map<CoTimers::timer_t, shared_ptr<CoTimerItem>> timers;
    mutex mtx;
    unique_ptr<thread> thd;
    bool running = true;
};

CoTimers::CoTimers(double interval)
{
    NLUA_CLASS_CONSTRUCT()
    d_ptr->interval = interval;
}

CoTimers::~CoTimers() noexcept {
    NLUA_CLASS_DESTORY()}

CoTimers::timer_t CoTimers::add(double interval, int repeat, tick_t cb)
{
    NLUA_AUTOGUARD(d_ptr->mtx)

    auto t = make_shared<CoTimerItem>();
    t->id = ++d_ptr->counter;
    t->seconds = interval;
    t->left = interval;
    t->tick = move(cb);
    t->repeat = repeat;
    d_ptr->timers.insert(make_pair(t->id, t));

    return t->id;
}

void CoTimers::cancel(timer_t id)
{
    NLUA_AUTOGUARD(d_ptr->mtx)
    d_ptr->timers.erase(id);
}

// 最小粒度定时器
NLUA_SINGLETON_IMPL(CoTimers, 0.1);

Timer::timer_t Timer::SetTimeout(double time, tick_t cb)
{
    return CoTimers::shared().add(time, 1, cb);
}

void Timer::CancelTimeout(timer_t id)
{
    CoTimers::shared().cancel(id);
}

Timer::timer_t Timer::SetInterval(double time, tick_t cb)
{
    return CoTimers::shared().add(time, -1, cb);
}

void Timer::CancelInterval(timer_t id)
{
    CoTimers::shared().cancel(id);
}

NLUA_END
