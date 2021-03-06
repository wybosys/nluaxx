﻿#include "nlua++.hpp"
#define __NLUA_PRIVATE__
#include "value.hpp"
#include "private.hpp"

#include <cross/cross.hpp>
#include <cross/str.hpp>
#include <cross/fs.hpp>
#include <cross/stringbuilder.hpp>

USE_STL;
USE_CROSS;

NLUA_BEGIN

NNT_SINGLETON_IMPL(Context);

void Context::_shared_init()
{
    // pass
}

Context::Context()
{
    NNT_CLASS_CONSTRUCT()
}

Context::~Context()
{
    NNT_CLASS_DESTROY()
}

Context &Context::attach(void *_l)
{
    d_ptr->attach((lua_State *) _l);
    return *this;
}

Context &Context::create()
{
    d_ptr->create();
    return *this;
}

bool Context::load(string const &file)
{
    auto L = d_ptr->L;
    NLUA_AUTOSTACK(L);

    auto tgt = d_ptr->find_file(file);

    if (tgt.empty()) {
        NLUA_ERROR("没有找到文件 " << file);
        return false;
    }

    int s = luaL_dofile(L, tgt.c_str());
    if (LUA_OK != s) {
        NLUA_ERROR("加载文件失败 " << file);
        return false;
    }

    NLUA_DEBUG("加载文件成功" << file);

    return true;
}

bool Context::load(void *buf, size_t len)
{
    auto L = d_ptr->L;
    NLUA_AUTOSTACK(L);

    int s = luaL_loadbuffer(L, (char const *) buf, len, "load-buffer");
    if (LUA_OK != s) {
        NLUA_ERROR("加载缓存失败");
        return false;
    }

    s = lua_pcall(L, 0, LUA_MULTRET, 0);
    if (LUA_OK != s) {
        NLUA_ERROR("运行缓存内容失败");
        return false;
    }

    NLUA_DEBUG("加载缓存成功");

    return true;
}

void Context::add_package_path(string const &dir)
{
    auto fdir = absolute(dir);
    if (fdir.empty())
        return;

    auto L = d_ptr->L;
    NLUA_AUTOSTACK(L);

    lua_getglobal(L, "package");
    int pkgid = lua_gettop(L);

    lua_getfield(L, -1, "path");
    string cur = lua_tostring(L, -1);

#ifdef WIN32
    string d = fdir + "\\?.lua";
    string di = fdir + "\\?\\init.lua";
#else
    string d = fdir + "/?.lua";
    string di = fdir + "/?/init.lua";
#endif

    auto curs = explode(cur, ";");
    if (find(curs.begin(), curs.end(), d) == curs.end()) {
        curs.emplace_back(d);
        curs.emplace_back(di);

        cur = implode(curs, ";");
        lua_pushstring(L, cur.c_str());
        lua_setfield(L, pkgid, "path");

        d_ptr->update_package_paths(&curs);
    }
}

void Context::add_cpackage_path(string const &dir)
{
    auto L = d_ptr->L;
    NLUA_AUTOSTACK(L);

    lua_getglobal(L, "package");
    int pkgid = lua_gettop(L);

    lua_getfield(L, -1, "cpath");
    string cur = lua_tostring(L, -1);

#ifdef WIN32
    string d = absolute(dir) + "\\?.dll";
#else
    string d = absolute(dir) + "/?.so";
#endif

    auto curs = explode(cur, ";");
    if (find(curs.begin(), curs.end(), d) == curs.end()) {
        curs.emplace_back(d);

        cur = implode(curs, ";");
        lua_pushstring(L, cur.c_str());
        lua_setfield(L, pkgid, "path");

        d_ptr->update_cpackage_paths(&curs);
    }
}

void Context::clear()
{
#if NLUA_MT

    // 检查是否有死锁
    if (d_ptr->pvd_worker->mtx.try_lock()) {
        d_ptr->pvd_worker->mtx.unlock();
    }
    else {
        NLUA_ERROR("检测到NLUA层死锁");
        return;
    }

#endif

    // 清空
    d_ptr->clear();
}

void Context::add(class_type &cls)
{
    d_ptr->classes[cls->name] = cls;
}

void Context::add(module_type &m)
{
    auto fnd = d_ptr->modules.find(m->name);
    if (fnd != d_ptr->modules.end()) {
        fnd->second->merge(*m);
    }
    else {
        d_ptr->modules[m->name] = m;
    }
}

void Context::declare()
{
    for (auto &e : d_ptr->classes) {
        e.second->declare_in(*this);
    }

    for (auto &e : d_ptr->modules) {
        e.second->declare_in(*this);
    }
}

return_type Context::invoke(string const &fname, args_type const &args)
{
    auto L = d_ptr->L;
    NLUA_AUTOSTACK(L);

    lua_pushcfunction(L, ContextPrivate::Traceback);
    int tbid = lua_gettop(L);

    lua_getglobal(L, fname.c_str());
    if (lua_isnil(L, -1)) {
        NLUA_ERROR("没有找到lua函数 " << fname);
        return nullptr;
    }

    for (auto &e : args) {
        push(*e, L);
    }

    int s = lua_pcall(L, args.size(), 1, tbid);
    if (LUA_OK != s)
        return nullptr;
    return at(L, -1);
}

return_type Context::invoke(string const &name, Variant const &v0)
{
    return invoke(name, {&v0});
}

return_type Context::invoke(string const &name, Variant const &v0, Variant const &v1)
{
    return invoke(name, {&v0, &v1});
}

return_type
Context::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2)
{
    return invoke(name, {&v0, &v1, &v2});
}

return_type Context::invoke(string const &name,
                            Variant const &v0,
                            Variant const &v1,
                            Variant const &v2,
                            Variant const &v3)
{
    return invoke(name, {&v0, &v1, &v2, &v3});
}

return_type Context::invoke(string const &name,
                            Variant const &v0,
                            Variant const &v1,
                            Variant const &v2,
                            Variant const &v3,
                            Variant const &v4)
{
    return invoke(name, {&v0, &v1, &v2, &v3, &v4});
}

return_type Context::invoke(string const &name,
                            Variant const &v0,
                            Variant const &v1,
                            Variant const &v2,
                            Variant const &v3,
                            Variant const &v4,
                            Variant const &v5)
{
    return invoke(name, {&v0, &v1, &v2, &v3, &v4, &v5});
}

return_type Context::invoke(string const &name,
                            Variant const &v0,
                            Variant const &v1,
                            Variant const &v2,
                            Variant const &v3,
                            Variant const &v4,
                            Variant const &v5,
                            Variant const &v6)
{
    return invoke(name, {&v0, &v1, &v2, &v3, &v4, &v5, &v6});
}

return_type Context::invoke(string const &name,
                            Variant const &v0,
                            Variant const &v1,
                            Variant const &v2,
                            Variant const &v3,
                            Variant const &v4,
                            Variant const &v5,
                            Variant const &v6,
                            Variant const &v7)
{
    return invoke(name, {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7});
}

return_type Context::invoke(string const &name,
                            Variant const &v0,
                            Variant const &v1,
                            Variant const &v2,
                            Variant const &v3,
                            Variant const &v4,
                            Variant const &v5,
                            Variant const &v6,
                            Variant const &v7,
                            Variant const &v8)
{
    return invoke(name, {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8});
}

return_type Context::invoke(string const &name,
                            Variant const &v0,
                            Variant const &v1,
                            Variant const &v2,
                            Variant const &v3,
                            Variant const &v4,
                            Variant const &v5,
                            Variant const &v6,
                            Variant const &v7,
                            Variant const &v8,
                            Variant const &v9)
{
    return invoke(name, {&v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, &v9});
}

void Context::lock()
{
#if NLUA_MT
    d_ptr->pvd_worker->mtx.lock();
#else
    d_ptr->mtx_global.lock();
#endif
}

void Context::unlock()
{
#if NLUA_MT
    d_ptr->pvd_worker->mtx.unlock();
#else
    d_ptr->mtx_global.unlock();
#endif
}

NLUA_END
