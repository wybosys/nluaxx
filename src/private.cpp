#include "nlua++.hpp"
#define __NLUA_PRIVATE__
#include "private.hpp"
#include "value.hpp"

#include <cross/cross.hpp>
#include <cross/str.hpp>
#include <cross/fs.hpp>

NLUA_BEGIN

USE_STL;
USE_CROSS;

ContextAutoGuard::ContextAutoGuard()
{
    tid = get_thread_id();

    if (!MainL)
        return;
    L = lua_newthread(MainL);
}

ContextAutoGuard::~ContextAutoGuard()
{
    // pass
}

lua_State *ContextAutoGuard::MainL = nullptr;
thread_local ContextAutoGuard ContextAutoGuard::Tls;

ContextPrivate::ContextPrivate()
    : refId(1) 
{
    // pass
}

ContextPrivate::~ContextPrivate() 
{
    clear();
    
    if (_freel) {
        lua_close(L);
    }

    L = nullptr;
    _freel = false;
}

void ContextPrivate::clear() 
{
    classes.clear();
    modules.clear();
    
    refId = 1;
    refClassFuncs.clear();
    refFuncs.clear();

    refSingletonId = 1;
}

void ContextPrivate::create() 
{
    if (ContextAutoGuard::Tls.ismain)
    {
        // 如果时主线程，则为关闭重建的流程
        if (L && _freel) {
            lua_close(L);
        }

        L = luaL_newstate();
        luaL_openlibs(L);
        _freel = true;

        ContextAutoGuard::MainL = L;
        ContextAutoGuard::Tls.L = L;
    }
    else
    {
        // 如果主线程L已经存在，则当前为其他线程，走获取逻辑
        if (ContextAutoGuard::MainL)
        {
            L = ContextAutoGuard::Tls.L;
            _freel = false;
        }
        else
        {
            // 不存在主线程L，仍为主线程逻辑
            if (L && _freel) {
                lua_close(L);
            }

            L = luaL_newstate();
            luaL_openlibs(L);
            _freel = true;

            // 绑定主线程环境
            ContextAutoGuard::MainL = L;
            ContextAutoGuard::Tls.L = L;
            ContextAutoGuard::Tls.ismain = true;
        }
    }

#ifdef WIN32
    // windows中需要额外设置lua的package.path保证可以拿到全局安装的库
    char *val;
    size_t len;
    errno_t err = _dupenv_s(&val, &len, "LUA_DEV");
    if (!err)
    {
        NLUA_AUTOSTACK(L);

        // 拼上查找目录
        vector<string> dirs = {
"\\?.lua",
"\\?\\init.lua",
"\\lua\\?.lua",
"\\lua\\?\\init.lua" };
        for (auto &e : dirs)
        {
            e = val + e;
        }

        vector<string> cdirs = {
"\\?.dll",
"\\loadall.dll",
"\\clibs\\?.dll",
"\\clibs\\loadall.dll",
"\\?51.dll",
"\\clibs\\?51.dll" };
        for (auto &e : cdirs)
        {
            e = val + e;
        }

        lua_getglobal(L, "package");
        int pkgid = lua_gettop(L);

        lua_getfield(L, pkgid, "path");
        string cur = lua_tostring(L, -1);
        cur += ";" + implode(dirs, ";");
        lua_pushlstring(L, cur.c_str(), cur.length());
        lua_setfield(L, pkgid, "path");

        lua_getfield(L, pkgid, "cpath");
        cur = lua_tostring(L, -1);
        cur += ";" + implode(cdirs, ";");
        lua_pushlstring(L, cur.c_str(), cur.length());
        lua_setfield(L, pkgid, "cpath");
        
        free(val);
    }
#endif
}

void ContextPrivate::attach(lua_State *_l) 
{
    if (_l == L)
        return;

    if (L && _freel) {
        lua_close(L);
    }

    L = _l;
    _freel = false;

    if (L) 
    {
        if (ContextAutoGuard::Tls.ismain) 
        {
            ContextAutoGuard::MainL = L;
            ContextAutoGuard::Tls.L = L;
        }
        else
        {
            if (ContextAutoGuard::MainL)
            {
                ContextAutoGuard::Tls.L = L;
            }
            else
            {
                // 绑定主线程环境
                ContextAutoGuard::MainL = L;
                ContextAutoGuard::Tls.L = L;
                ContextAutoGuard::Tls.ismain = true;
            }
        }
    }
}

int ContextPrivate::Traceback(lua_State *L) 
{
    if (!lua_isstring(L, 1))
        return 1;
    string msg = lua_tostring(L, 1);
    cerr << "捕获错误 " + msg << endl;
    return 0;
}

string ContextPrivate::find_file(string const &file) 
{
    if (isfile(file))
        return file;

    if (package_paths.empty()) {
        update_package_paths(nullptr);
    }

    for (auto e : package_paths) {
        e = e + PATH_DELIMITER + file;
        if (isfile(e))
            return e;
    }
    return "";
}

void ContextPrivate::update_package_paths(vector<string> const *curs) 
{
    if (curs == nullptr) {
        NLUA_AUTOSTACK(L);

        lua_getglobal(L, "package");
        int pkgid = lua_gettop(L);

        lua_getfield(L, -1, "path");
        string cur = lua_tostring(L, -1);
        auto t = explode(cur, ";");
        update_package_paths(&t);
        return;
    }

    package_paths.clear();
    for (auto cur : *curs) {
#ifdef WIN32
        if (endwith(cur, "?.lua"))
        {
            cur = cur.substr(0, cur.length() - 6);
        }
        else if (endwith(cur, "?\\init.lua"))
        {
            cur = cur.substr(0, cur.length() - 11);
        }
#else
        if (endwith(cur, "?.lua")) {
            cur = cur.substr(0, cur.length() - 6);
        }
        else if (endwith(cur, "?/init.lua")) {
            cur = cur.substr(0, cur.length() - 11);
        }
#endif
        string c = absolute(cur);
        if (isdirectory(c)) {
            if (find(package_paths.begin(), package_paths.end(), c) == package_paths.end()) {
                package_paths.emplace_back(c);
                // cout << "package path: " << c << endl;
            }
        }
    }
}

void ContextPrivate::update_cpackage_paths(vector<string> const *curs) 
{
    if (curs == nullptr) {
        NLUA_AUTOSTACK(L);

        lua_getglobal(L, "package");
        int pkgid = lua_gettop(L);

        lua_getfield(L, -1, "cpath");
        string cur = lua_tostring(L, -1);
        auto t = explode(cur, ";");
        update_cpackage_paths(&t);
        return;
    }

    cpackage_paths.clear();
    for (auto cur : *curs) {
#ifdef WIN32
        if (endwith(cur, "?.dll")) {
            cur = cur.substr(0, cur.length() - 5);
        }
#else
        if (endwith(cur, "?.so")) {
            cur = cur.substr(0, cur.length() - 5);
        }
#endif
        string c = absolute(cur);
        if (isdirectory(c)) {
            if (find(cpackage_paths.begin(), cpackage_paths.end(), c) == cpackage_paths.end()) {
                cpackage_paths.emplace_back(c);
                // cout << "cpackage path: " << c << endl;
            }
        }
    }
}

NLUA_END