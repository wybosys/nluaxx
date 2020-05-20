#ifndef __NLUA_TIMER_H_INCLUDED
#define __NLUA_TIMER_H_INCLUDED

NLUA_BEGIN

NLUA_CLASS_PREPARE(TimeCounter)

// 计时器
class TimeCounter {
NLUA_CLASS_DECL(TimeCounter)

public:

    TimeCounter();

    ~TimeCounter();

    // 启动定时
    void start();

    // 过去的秒
    double seconds(bool reset = true);
};

class Time {
public:

    // 休眠
    static void Sleep(double seconds);
};

NLUA_CLASS_PREPARE(CoTimers)

// 最小粒度定时器
class CoTimers {
NLUA_CLASS_DECL(CoTimers)
public:

    explicit CoTimers(double interval);

    ~CoTimers() noexcept;

    typedef function<void()> tick_t;
    typedef uint timer_t;

    // 添加一个定时器
    timer_t add(double interval, int repeat, tick_t cb);

    // 取消定时器
    void cancel(timer_t);
};

// 定时器
class Timer {
public:

    typedef CoTimers::tick_t tick_t;
    typedef CoTimers::timer_t timer_t;

    // 延迟调用
    static timer_t SetTimeout(double time, tick_t);

    // 取消延迟调用
    static void CancelTimeout(timer_t);

    // 按时间重复
    static timer_t SetInterval(double time, tick_t);

    // 取消按时间重复
    static void CancelInterval(timer_t);

};

NLUA_END

#endif
