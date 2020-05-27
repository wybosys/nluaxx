#ifndef __NLUA_H_INCLUDED
#define __NLUA_H_INCLUDED

#ifdef WIN32
#include "stdafx.h"
#endif

#ifndef NLUA_NS
#define NLUA_NS nlua
#endif

#define NLUA_BEGIN    \
    namespace NLUA_NS \
    {
#define NLUA_END }
#define USE_NLUA using namespace NLUA_NS;

#include <string>
#include <memory>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <functional>

#include "com++.h"
#include "macro.h"

#define NLUA_STATIC_INVOKE(func) \
[&](args_type const& args)->return_type { \
return make_shared<Variant>(com::function_call<decltype(&func)>()(&func, args)); \
}

NLUA_BEGIN

using namespace ::std;

// 执行main循环
extern int Exec();

class error : public exception
{
public:
    explicit error(int code, string const &msg = "") : _code(code), _msg(msg) {}

    virtual const char *what() const throw()
    {
        return _msg.c_str();
    }

private:
    int _code;
    string _msg;
};

NLUA_END

#endif
