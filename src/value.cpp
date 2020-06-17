#include "nlua++.hpp"
#define __NLUA_PRIVATE__
#include "value.hpp"

NLUA_BEGIN

extern void push(Variant const *v, lua_State *L) {
    if (v == nullptr) {
        lua_pushnil(L);
        return;
    }
    push(*v, L);
}

void push(Variant const &v, lua_State *L) {
    switch (v.vt) {
        case Variant::VT::NIL:
            lua_pushnil(L);
            break;
        case Variant::VT::POINTER:
            lua_pushlightuserdata(L, v.toPointer());
            break;
        case Variant::VT::NUMBER:
            lua_pushnumber(L, v.toNumber());
            break;
        case Variant::VT::INTEGER:
            lua_pushinteger(L, v.toInteger());
            break;
        case Variant::VT::STRING: {
            auto const &s = v.toString();
            lua_pushlstring(L, s.c_str(), s.length());
        }
            break;
        case Variant::VT::BOOLEAN:
            lua_pushboolean(L, v.toBool());
            break;
        case Variant::VT::OBJECT:
            lua_pushvalue(L, v.toInteger());
            break;
    }
}

return_type at(lua_State *L, int n) {
    int t = lua_gettop(L);
    auto const typ = lua_type(L, n);
    switch (typ)
    {
    case LUA_TSTRING:
        return make_shared<Variant>(string(lua_tostring(L, n), lua_strlen(L, n)));
    case LUA_TBOOLEAN:
        return make_shared<Variant>(!!lua_toboolean(L, n));
    case LUA_TNUMBER:
        return make_shared<Variant>(lua_tonumber(L, n));
    case LUA_TNIL:
        return make_shared<Variant>();
    case LUA_TFUNCTION:
    case LUA_TTABLE:
        return make_shared<Variant>(to_object(L, n));
    default:
        NLUA_ERROR("Value::at 遇到一个不支持的lua变量类型 " << typ);
        break;
    }
    return make_shared<Variant>();
}

NLUA_END
