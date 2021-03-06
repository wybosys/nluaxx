﻿#ifndef __NLUA_VALUE_H_INCLUDED
#define __NLUA_VALUE_H_INCLUDED

#include "variant.hpp"
#include "liblua.hpp"

NLUA_BEGIN

extern void push(Variant const *, lua_State *);

extern void push(Variant const &, lua_State *);

extern return_type at(lua_State *, int n);

// 将N封装成Object
extern self_type to_object(lua_State *, int n);

class AutoStackRecover
{
public:
    explicit AutoStackRecover(lua_State *L)
        : L(L)
    {
        _old = lua_gettop(L);
    }

    ~AutoStackRecover()
    {
        recover();
    }

    void recover() {
        if (L) {
            lua_settop(L, _old);
            L = nullptr;
        }
    }

private:
    lua_State *L;
    int _old;
};

#define NLUA_AUTOSTACK(L, ...) AutoStackRecover _NNT_COMBINE(__nlua_autostack_, __LINE__)(L);

NLUA_END

#endif
