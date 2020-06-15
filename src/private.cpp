#include "nlua++.hpp"
#include "private.hpp"
#include "value.hpp"

#include <cross/cross.hpp>
#include <cross/str.hpp>
#include <cross/fs.hpp>

NLUA_BEGIN

USE_STL
USE_CROSS

ContextPrivate::ContextPrivate()
    : refId(1) {
}

ContextPrivate::~ContextPrivate() {
    clear();
    
    if (_freel) {
        lua_close(L);
    }
    L = nullptr;
    _freel = false;
}

void ContextPrivate::clear() {
    classes.clear();
    modules.clear();
    
    refId.store(1);
    refClassFuncs.clear();
    refFuncs.clear();
}

void ContextPrivate::create() {
    if (L && _freel) {
        lua_close(L);
    }
    
    L = luaL_newstate();
    luaL_openlibs(L);
    _freel = true;

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

void ContextPrivate::attach(lua_State *_l) {
    if (_l == L)
        return;

    if (L && _freel) {
        lua_close(L);
    }

    L = _l;
    _freel = false;
}

int ContextPrivate::Traceback(lua_State *L) {
    if (!lua_isstring(L, 1))
        return 1;
    string msg = lua_tostring(L, 1);
    cerr << "捕获错误 " + msg << endl;
    return 0;
}

string ContextPrivate::find_file(string const &file) {
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

void ContextPrivate::update_package_paths(vector<string> const *curs) {
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

void ContextPrivate::update_cpackage_paths(vector<string> const *curs) {
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