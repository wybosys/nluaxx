#include "core.h"
#include "value.h"

NLUA_BEGIN

extern void push(Variant const *v, lua_State *L) {
    if (v == nullptr) {
        lua_pushnil(L);
        return;
    }
    push(*v, L);
}

void push(Variant const &v, lua_State *L) {
    switch (v.type()) {
        default:
            lua_pushnil(L);
            break;
        case VariantType::POINTER:
            lua_pushlightuserdata(L, v.toPointer());
            break;
        case VariantType::DECIMAL:
            lua_pushnumber(L, v.toNumber());
            break;
        case VariantType::INTEGER:
            lua_pushinteger(L, (lua_Integer)v.toInteger());
            break;
        case VariantType::STRING: {
            auto s = v.toString();
            lua_pushlstring(L, s.c_str(), s.length());
        }
            break;
        case VariantType::BOOLEAN:
            lua_pushboolean(L, v.toBoolean());
            break;
    }
}

return_type at(lua_State *L, int n) {
    int t = lua_gettop(L);
    if (lua_isnil(L, n))
        return nullptr;
    if (lua_isnumber(L, n))
        return make_shared<Variant>(luaL_checknumber(L, n));
    if (lua_isboolean(L, n))
        return make_shared<Variant>((bool) luaL_checkint(L, n));
    if (lua_isstring(L, n))
        return make_shared<Variant>((string) luaL_checkstring(L, n));
    return make_shared<Variant>();
}

NLUA_END

