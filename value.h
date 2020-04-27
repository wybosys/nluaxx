#ifndef __NLUA_VALUE_H_INCLUDE
#define __NLUA_VALUE_H_INCLUDE

#include "variant.h"
#include "nlua++.h"
#include <luajit-2.1/lua.hpp>

NLUA_BEGIN

extern void push(Variant const *, lua_State *);

extern void push(Variant const &, lua_State *);

extern return_type at(lua_State *, int n);

class AutoStackRecover {
public:

    explicit AutoStackRecover(lua_State *L)
            : L(L) {
        _old = lua_gettop(L);
    }

    ~AutoStackRecover() {
        lua_settop(L, _old);
    }

private:
    lua_State *L;
    int _old;
};

#define NLUA_AUTOSTACK(L, ...) AutoStackRecover _NLUA_COMBINE(__nlua_autostack_, __LINE__)(L);

NLUA_END

#endif
