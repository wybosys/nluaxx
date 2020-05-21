#ifndef __NLUA_LIBLUA_H_INCLUDED
#define __NLUA_LIBLUA_H_INCLUDED

#ifdef NLUA_JIT

#include <luajit-2.1/lua.hpp>

#endif

#ifdef NLUA_51
#include <lua5.1/lua.hpp>
#define LUA_OK 0
#endif

#ifdef NLUA_52
#include <lua5.2/lua.hpp>
#endif

#ifdef NLUA_53
#define LUA_COMPAT_APIINTCASTS
#include <lua5.3/lua.hpp>
#endif

#endif
