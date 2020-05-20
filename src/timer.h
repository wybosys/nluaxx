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

    static void Sleep(double seconds);
};

NLUA_END

#endif
