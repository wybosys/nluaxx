#ifndef __NLUA_LIBLUA_H_INCLUDED
#define __NLUA_LIBLUA_H_INCLUDED

#ifndef NLUA_LIBJIT
#define NLUA_LIBJIT luajit-2.1
#endif

#ifndef NLUA_LIB51
#define NLUA_LIB51 lua5.1
#endif

#ifndef NLUA_LIB52
#define NLUA_LIB52 lua5.2
#endif

#ifndef NLUA_LIB53
#define NLUA_LIB53 lua5.3
#endif

#define NLUA_INCLUDE(DIR, FILE) <DIR/FILE>

#ifdef NLUA_JIT
#include NLUA_INCLUDE(NLUA_LIBJIT, lua.hpp)
#define LUA_INCLUDED
#endif

#ifdef NLUA_51
#include NLUA_INCLUDE(NLUA_LIB51, lua.hpp)
#define LUA_OK 0
#define LUA_INCLUDED
#endif

#ifdef NLUA_52
#include NLUA_INCLUDE(NLUA_LIB52, lua.hpp)
#define LUA_INCLUDED
#endif

#ifdef NLUA_53
#define LUA_COMPAT_APIINTCASTS
#include NLUA_INCLUDE(NLUA_LIB53, lua.hpp)
#define LUA_INCLUDED
#endif

#ifndef LUA_INCLUDED
extern "C"
{
#include <luaconf.h>
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#define LUA_OK 0
}
#endif

#endif
