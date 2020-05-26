#include "core.h"
#include "nlua++.h"
#include "liblua.h"
#include "value.h"
#include "str.h"
#include <atomic>
#include <sstream>

NLUA_BEGIN

#define IMPFUNC_CHECK_ARGS(count)        \
    do                                   \
    {                                    \
        if (args.size() < count)         \
            throw error(-1, "缺少参数"); \
    } while (0);

typedef atomic<lua_Integer> lua_refid_type;
typedef function<return_type(lua_State *L, self_type &self, args_type const &)> luaref_classfunc_type;
typedef function<return_type(lua_State *L, args_type const &)> luaref_func_type;
typedef map<lua_Integer, luaref_classfunc_type> luaref_classfuncs_type;
typedef map<lua_Integer, luaref_func_type> luaref_funcs_type;

class ContextPrivate {
public:

    ContextPrivate()
            : refId(1) {
    }

    ~ContextPrivate() {
        clear();
    }

    void clear() {
        if (_freel) {
            lua_close(L);
        }
        L = nullptr;
        _freel = false;

        classes.clear();
        modules.clear();

        refId.store(1);
        refClassFuncs.clear();
        refFuncs.clear();
    }

    void create() {
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
                    "\\lua\\?\\init.lua"};
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
                    "\\clibs\\?51.dll"};
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

    void attach(lua_State *_l) {
        if (_l == L)
            return;

        if (L && _freel) {
            lua_close(L);
        }

        L = _l;
        _freel = false;
    }

    static int Traceback(lua_State *L) {
        if (!lua_isstring(L, 1))
            return 1;
        cerr << "捕获错误 " << luaL_checkstring(L, 1) << endl;
        return 0;
    }

    string find_file(string const &file) {
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

    void update_package_paths(vector<string> const *curs) {
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
            } else if (endwith(cur, "?/init.lua")) {
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

    void update_cpackage_paths(vector<string> const *curs) {
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
#ifndef WIN32
            if (endwith(cur, "?.dll")) {
                cur = cur.substr(0, cur.length() - 5);
            }
            string c = absolute(cur);
            if (isdirectory(c)) {
                if (find(cpackage_paths.begin(), cpackage_paths.end(), c) == cpackage_paths.end()) {
                    cpackage_paths.emplace_back(c);
                    // cout << "cpackage path: " << c << endl;
                }
            }
#else
            if (endwith(cur, "?.so"))
            {
                cur = cur.substr(0, cur.length() - 5);
            }
            path c = path::absolute(cur);
            if (path::isdirectory(c))
            {
                if (find(cpackage_paths.begin(), cpackage_paths.end(), c) == cpackage_paths.end())
                {
                    cpackage_paths.emplace_back(c);
                    // cout << "cpackage path: " << c << endl;
                }
            }
#endif
        }
    }

    lua_State *L = nullptr;
    bool _freel = false;

    vector<string> package_paths, cpackage_paths;

    // 当前声明中的类和模块
    Context::classes_type classes;
    Context::modules_type modules;

    // 保存lua实现的成员函数、类函数和C++实现之间的关系
    lua_refid_type refId;
    luaref_funcs_type refFuncs;
    luaref_classfuncs_type refClassFuncs;
};

static lua_State *GetContextL(Context &ctx) {
    return nnt::DPtr<Context, ContextPrivate>(&ctx)->L;
}

NNT_SINGLETON_IMPL(Context);

Context::Context() {
    NNT_CLASS_CONSTRUCT()
}

Context::~Context() {
    NNT_CLASS_DESTORY()
}

Context &Context::attach(void *_l) {
    d_ptr->attach((lua_State *) _l);
    return *this;
}

Context &Context::create() {
    d_ptr->create();
    return *this;
}

bool Context::load(string const &file) {
    auto L = d_ptr->L;
    NLUA_AUTOSTACK(L);

    auto tgt = d_ptr->find_file(file);
    // cout << tgt << endl;

    if (tgt.empty()) {
        cerr << "没有找到文件 " << file << endl;
        return false;
    }

    int s = luaL_dofile(L, tgt.c_str());
    if (LUA_OK != s) {
        cerr << "加载文件失败 " << file << endl;
        return false;
    }
    return true;
}

void Context::add_package_path(string const &dir) {
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

void Context::add_cpackage_path(string const &dir) {
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

void Context::clear() {
    d_ptr->clear();
}

void Context::add(class_type &cls) {
    d_ptr->classes[cls->name] = cls;
}

void Context::add(module_type &m) {
    auto fnd = d_ptr->modules.find(m->name);
    if (fnd != d_ptr->modules.end()) {
        fnd->second->merge(*m);
    } else {
        d_ptr->modules[m->name] = m;
    }
}

void Context::declare() {
    for (auto &e : d_ptr->classes) {
        e.second->declare_in(*this);
    }

    for (auto &e : d_ptr->modules) {
        e.second->declare_in(*this);
    }
}

return_type Context::invoke(string const &fname, args_type const &args) {
    auto L = d_ptr->L;
    NLUA_AUTOSTACK(L);

    lua_pushcfunction(L, ContextPrivate::Traceback);
    int tbid = lua_gettop(L);

    lua_getglobal(L, fname.c_str());
    if (lua_isnil(L, -1)) {
        cerr << "没有找到lua函数 " << fname << endl;
        return nullptr;
    }

    for (auto &e : args) {
        push(e, L);
    }

    int s = lua_pcall(L, args.size(), 1, tbid);
    if (LUA_OK != s)
        return nullptr;
    return at(L, -1);
}

return_type Context::invoke(string const &name, Variant const &v0) {
    return invoke(name, {v0});
}

return_type Context::invoke(string const &name, Variant const &v0, Variant const &v1) {
    return invoke(name, {v0, v1});
}

return_type Context::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2) {
    return invoke(name, {v0, v1, v2});
}

return_type Context::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2, Variant const &v3) {
    return invoke(name, {v0, v1, v2, v3});
}

return_type Context::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2, Variant const &v3, Variant const &v4) {
    return invoke(name, {v0, v1, v2, v3, v4});
}

return_type Context::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2, Variant const &v3, Variant const &v4, Variant const &v5) {
    return invoke(name, {v0, v1, v2, v3, v4, v5});
}

return_type Context::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2, Variant const &v3, Variant const &v4, Variant const &v5, Variant const &v6) {
    return invoke(name, {v0, v1, v2, v3, v4, v5, v6});
}

return_type Context::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2, Variant const &v3, Variant const &v4, Variant const &v5, Variant const &v6, Variant const &v7) {
    return invoke(name, {v0, v1, v2, v3, v4, v5, v6, v7});
}

return_type Context::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2, Variant const &v3, Variant const &v4, Variant const &v5, Variant const &v6, Variant const &v7, Variant const &v8) {
    return invoke(name, {v0, v1, v2, v3, v4, v5, v6, v7, v8});
}

return_type Context::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2, Variant const &v3, Variant const &v4, Variant const &v5, Variant const &v6, Variant const &v7, Variant const &v8, Variant const &v9) {
    return invoke(name, {v0, v1, v2, v3, v4, v5, v6, v7, v8, v9});
}

void Field::declare_in(Context &ctx) const {
    auto L = GetContextL(ctx);
    NLUA_AUTOSTACK(L);

    push(val.get(), L);
    lua_setglobal(L, name.c_str());
}

void Field::declare_in(Context &ctx, Class const &clz) const {
    auto L = GetContextL(ctx);
    NLUA_AUTOSTACK(L);

    int clzid = lua_gettop(L);

    lua_pushstring(L, name.c_str());
    push(val.get(), L);

    lua_rawset(L, clzid);
}

class ObjectPrivate {
public:

    // 如果传入自定义C数据结构，则self指针将指向该结构
    struct UserData {
        void *data = nullptr;
        bool freed = false; // 已经释放
    };

    // 当前栈
    lua_State *L = nullptr;

    // 当前栈上对象的id（local对象）
    int id = 0;

    // 对象名
    string name;

    typedef atomic<lua_Integer> lua_global_refid_type;
    static lua_global_refid_type RefId;
};

ObjectPrivate::lua_global_refid_type ObjectPrivate::RefId(1);

class FunctionPrivate {
public:

    static int ImpClassFunction(lua_State *L) {
        auto pctx = (ContextPrivate *) lua_topointer(L, lua_upvalueindex(1));
        auto id = lua_tointeger(L, lua_upvalueindex(2));

        auto fnd = pctx->refClassFuncs.find(id);
        if (fnd == pctx->refClassFuncs.end()) {
            cerr << "没有找到对应的回调函数" << endl;
            return 0;
        }

        return_type rv;
        self_type self = make_shared<Object>();
        self->d_ptr->id = 1; //  first argument (if any) is at index 1
        self->d_ptr->L = L;

        int count = lua_gettop(L) - 1; // last argument is at index lua_gettop(L)
        switch (count) {
            case 0: {
                rv = fnd->second(L, self, {});
            }
                break;
            case 1: {
                auto v0 = at(L, 2);
                rv = fnd->second(L, self, {*v0});
            }
                break;
            case 2: {
                auto v0 = at(L, 2);
                auto v1 = at(L, 3);
                rv = fnd->second(L, self, {*v0, *v1});
            }
                break;
            case 3: {
                auto v0 = at(L, 2);
                auto v1 = at(L, 3);
                auto v2 = at(L, 4);
                rv = fnd->second(L, self, {*v0, *v1, *v2});
            }
                break;
            case 4: {
                auto v0 = at(L, 2);
                auto v1 = at(L, 3);
                auto v2 = at(L, 4);
                auto v3 = at(L, 5);
                rv = fnd->second(L, self, {*v0, *v1, *v2, *v3});
            }
                break;
            case 5: {
                auto v0 = at(L, 2);
                auto v1 = at(L, 3);
                auto v2 = at(L, 4);
                auto v3 = at(L, 5);
                auto v4 = at(L, 6);
                rv = fnd->second(L, self, {*v0, *v1, *v2, *v3, *v4});
            }
                break;
            case 6: {
                auto v0 = at(L, 2);
                auto v1 = at(L, 3);
                auto v2 = at(L, 4);
                auto v3 = at(L, 5);
                auto v4 = at(L, 6);
                auto v5 = at(L, 7);
                rv = fnd->second(L, self, {*v0, *v1, *v2, *v3, *v4, *v5});
            }
                break;
            case 7: {
                auto v0 = at(L, 2);
                auto v1 = at(L, 3);
                auto v2 = at(L, 4);
                auto v3 = at(L, 5);
                auto v4 = at(L, 6);
                auto v5 = at(L, 7);
                auto v6 = at(L, 8);
                rv = fnd->second(L, self, {*v0, *v1, *v2, *v3, *v4, *v5, *v6});
            }
                break;
            case 8: {
                auto v0 = at(L, 2);
                auto v1 = at(L, 3);
                auto v2 = at(L, 4);
                auto v3 = at(L, 5);
                auto v4 = at(L, 6);
                auto v5 = at(L, 7);
                auto v6 = at(L, 8);
                auto v7 = at(L, 9);
                rv = fnd->second(L, self, {*v0, *v1, *v2, *v3, *v4, *v5, *v6, *v7});
            }
                break;
            case 9: {
                auto v0 = at(L, 2);
                auto v1 = at(L, 3);
                auto v2 = at(L, 4);
                auto v3 = at(L, 5);
                auto v4 = at(L, 6);
                auto v5 = at(L, 7);
                auto v6 = at(L, 8);
                auto v7 = at(L, 9);
                auto v8 = at(L, 10);
                rv = fnd->second(L, self, {*v0, *v1, *v2, *v3, *v4, *v5, *v6, *v7, *v8});
            }
                break;
            default:
                break;
        }

        if (rv) {
            push(*rv, L);
            return 1;
        }

        return 0;
    }

    static int ImpStaticFunction(lua_State *L) {
        auto pctx = (ContextPrivate *) lua_topointer(L, lua_upvalueindex(1));
        auto id = lua_tointeger(L, lua_upvalueindex(2));

        auto fnd = pctx->refFuncs.find(id);
        if (fnd == pctx->refFuncs.end()) {
            cerr << "没有找到对应的回调函数" << endl;
            return 0;
        }

        return_type rv;
        int count = lua_gettop(L) - 1;
        switch (count) {
            case 0: {
                rv = fnd->second(L, {});
            }
                break;
            case 1: {
                auto v0 = at(L, 2);
                rv = fnd->second(L, {*v0});
            }
                break;
            case 2: {
                auto v0 = at(L, 2);
                auto v1 = at(L, 3);
                rv = fnd->second(L, {*v0, *v1});
            }
                break;
            case 3: {
                auto v0 = at(L, 2);
                auto v1 = at(L, 3);
                auto v2 = at(L, 4);
                rv = fnd->second(L, {*v0, *v1, *v2});
            }
                break;
            case 4: {
                auto v0 = at(L, 2);
                auto v1 = at(L, 3);
                auto v2 = at(L, 4);
                auto v3 = at(L, 5);
                rv = fnd->second(L, {*v0, *v1, *v2, *v3});
            }
                break;
            case 5: {
                auto v0 = at(L, 2);
                auto v1 = at(L, 3);
                auto v2 = at(L, 4);
                auto v3 = at(L, 5);
                auto v4 = at(L, 6);
                rv = fnd->second(L, {*v0, *v1, *v2, *v3, *v4});
            }
                break;
            case 6: {
                auto v0 = at(L, 2);
                auto v1 = at(L, 3);
                auto v2 = at(L, 4);
                auto v3 = at(L, 5);
                auto v4 = at(L, 6);
                auto v5 = at(L, 7);
                rv = fnd->second(L, {*v0, *v1, *v2, *v3, *v4, *v5});
            }
                break;
            case 7: {
                auto v0 = at(L, 2);
                auto v1 = at(L, 3);
                auto v2 = at(L, 4);
                auto v3 = at(L, 5);
                auto v4 = at(L, 6);
                auto v5 = at(L, 7);
                auto v6 = at(L, 8);
                rv = fnd->second(L, {*v0, *v1, *v2, *v3, *v4, *v5, *v6});
            }
                break;
            case 8: {
                auto v0 = at(L, 2);
                auto v1 = at(L, 3);
                auto v2 = at(L, 4);
                auto v3 = at(L, 5);
                auto v4 = at(L, 6);
                auto v5 = at(L, 7);
                auto v6 = at(L, 8);
                auto v7 = at(L, 9);
                rv = fnd->second(L, {*v0, *v1, *v2, *v3, *v4, *v5, *v6, *v7});
            }
                break;
            case 9: {
                auto v0 = at(L, 2);
                auto v1 = at(L, 3);
                auto v2 = at(L, 4);
                auto v3 = at(L, 5);
                auto v4 = at(L, 6);
                auto v5 = at(L, 7);
                auto v6 = at(L, 8);
                auto v7 = at(L, 9);
                auto v8 = at(L, 10);
                rv = fnd->second(L, {*v0, *v1, *v2, *v3, *v4, *v5, *v6, *v7, *v8});
            }
                break;
            default:
                break;
        }

        if (rv) {
            push(*rv, L);
            return 1;
        }

        return 0;
    }
};

Function::Function() {
    NNT_CLASS_CONSTRUCT()
}

Function::~Function() {
    NNT_CLASS_DESTORY()
}

void Function::declare_in(Context &ctx) const {
    auto L = GetContextL(ctx);
    NLUA_AUTOSTACK(L);

    auto pctx = nnt::DPtr<Context, ContextPrivate>(&ctx);
    auto id = pctx->refId.fetch_add(1);

    // 注册到全局对照表中，用于激活函数时查找真正的执行函数
    pctx->refFuncs[id] = [=](lua_State *L, args_type const &args) -> return_type {
        try {
            return this->func(args);
        }
        catch (exception &e) {
            luaL_error(L, e.what());
        }
        return nullptr;
    };

    lua_pushlightuserdata(L, pctx);
    lua_pushinteger(L, id);
    lua_pushcclosure(L, private_class_type::ImpStaticFunction, 2);

    lua_setglobal(L, name.c_str());
}

void Function::declare_in(Context &ctx, Class const &clz) const {
    auto L = GetContextL(ctx);
    NLUA_AUTOSTACK(L);

    auto pctx = nnt::DPtr<Context, ContextPrivate>(&ctx);
    auto id = pctx->refId.fetch_add(1);

    // 绑定到类
    int clzid = lua_gettop(L);
    lua_pushstring(L, name.c_str());
    lua_pushlightuserdata(L, pctx);
    lua_pushinteger(L, id);

    if (classfunc) {
        // 注册到全局对照表中，用于激活函数时查找真正的执行函数
        pctx->refClassFuncs[id] = [=](lua_State *L, self_type &self, args_type const &args) -> return_type {
            try {
                return this->classfunc(self, args);
            }
            catch (exception &e) {
                luaL_error(L, e.what());
            }
            return nullptr;
        };
        lua_pushcclosure(L, private_class_type::ImpClassFunction, 2);
    } else {
        // 注册静态函数
        pctx->refFuncs[id] = [=](lua_State *L, args_type const &args) -> return_type {
            try {
                return this->func(args);
            }
            catch (exception &e) {
                luaL_error(L, e.what());
            }
            return nullptr;
        };
        lua_pushcclosure(L, private_class_type::ImpStaticFunction, 2);
    }

    lua_rawset(L, clzid);
}

class ClassPrivate {
public:

    ClassPrivate() {
        // pass
    }

    // 仅输出定义段
    void body_declare_in(Context &ctx, Class const &_curclass) {
        auto L = GetContextL(ctx);
        NLUA_AUTOSTACK(L);

        for (auto &e : supers) {
            if (e.clazz) {
                e.clazz->d_ptr->body_declare_in(ctx, _curclass);
            } else if (e.object) {
                Object t; // 获得当前类对象，将父类进行keyvalue设置
                t.d_ptr->id = lua_gettop(L);
                e.object->setfor(t);
            }
        }

        for (auto &e:inits) {
            e->declare_in(ctx, _curclass);
        }

        if (fini) {
            fini->declare_in(ctx, _curclass);
        }

        for (auto &e : functions) {
            e.second->declare_in(ctx, _curclass);
        }

        for (auto &e : fields) {
            e.second->declare_in(ctx, _curclass);
        }
    }

    // 实例化类型
    static int ImpNew(lua_State *L) {
        // NLUA_AUTOSTACK(L); 会导致实例化的栈被弹出

        int clzid = 1;
        int args = lua_gettop(L) - 1;

        // 构造metatable
        lua_newtable(L);
        int metaid = lua_gettop(L);

        // 设置析构函数
        lua_pushstring(L, "delete");
        lua_rawget(L, clzid);
        const bool has_fini = lua_iscfunction(L, -1);
        if (has_fini) {
            lua_pushstring(L, "__gc");
            lua_pushvalue(L, -2);
            lua_rawset(L, metaid);
        }
        lua_pop(L, 1);

        // {__index=x}
        lua_pushstring(L, "__index");
        lua_pushvalue(L, clzid);
        lua_rawset(L, metaid);

        // instance
        if (has_fini) {
            // 申请一小段的内存
            lua_newuserdata(L, sizeof(ObjectPrivate::UserData));
            auto ud = (ObjectPrivate::UserData *) lua_touserdata(L, -1);
            ud->data = nullptr;
            ud->freed = false;
        } else {
            lua_newtable(L);
        }
        int objid = lua_gettop(L);

        // 绑定原型到对象
        lua_pushvalue(L, metaid);
        lua_setmetatable(L, objid);

        // 根据传入的参数个数，调用构造函数
        ostringstream oss;
        oss << "__init" << args << "__";
        string init = oss.str();

        // 判断构造函数是否存在
        lua_pushlstring(L, init.c_str(), init.length());
        lua_rawget(L, clzid);
        if (lua_iscfunction(L, -1)) {
            lua_pushcfunction(L, ContextPrivate::Traceback);
            int tbid = lua_gettop(L);

            // 压入构造函数
            lua_pushvalue(L, -2);

            // 找到了构造函数, 开始调用
            lua_pushvalue(L, objid);

            // 输入参数
            for (int i = 0; i < args; ++i) {
                lua_pushvalue(L, i + 2); // 1 号位为objid，从2开始才是参数
            }

            int s = lua_pcall(L, args + 1, 0, tbid);
            if (LUA_OK != s) {
                // 初始化失败，传入空
                lua_pushnil(L);
                return 1;
            }
        }

        // 将实例对象放到栈顶
        lua_pushvalue(L, objid);
        return 1;
    }

    // 调用原始的new函数实例化
    static void CallSingletonNew(lua_State *L) {
        // 1 clzid
        lua_pushcfunction(L, ContextPrivate::Traceback);

        // 2 tbid
        lua_pushstring(L, "__singleton_new__");
        lua_rawget(L, 1);
        lua_pushvalue(L, 1);

        int s = lua_pcall(L, 1, 1, 2);
        if (s != LUA_OK) {
            lua_pushnil(L);
        }
    }

    // 析构函数实现
    static int ImpDestroy(lua_State *L) {
        // self 1
        // 如果已经释放，则不调用
        auto ud = (ObjectPrivate::UserData *) lua_touserdata(L, -1);
        if (ud->freed)
            return 0;

        lua_pushcfunction(L, ContextPrivate::Traceback);
        // tbid 2

        // 从metatable提取fini函数
        lua_getmetatable(L, 1);
        // meta 3

        lua_pushstring(L, "__index");
        lua_rawget(L, 3);
        // index 4

        // 调用对象的fini函数
        lua_pushstring(L, "__fini__");
        lua_rawget(L, 4);
        // func 4

        lua_pushvalue(L, 1);
        lua_pcall(L, 1, 0, 2);

        ud->data = nullptr;
        ud->freed = true;
        return 0;
    }

    // 单件实现
    static int ImpGetSingleton(lua_State *L) {
        lua_pushstring(L, "__shared");
        lua_rawget(L, 1);
        if (lua_isnil(L, -1)) {
            // 恢复堆栈
            lua_pop(L, 1);

            // 调用new函数
            CallSingletonNew(L);

            int objid = lua_gettop(L);

            // 设置到__shared上
            lua_pushstring(L, "__shared");
            lua_pushvalue(L, objid);
            lua_rawset(L, 1);

            // 返回
            lua_pushvalue(L, objid);

            // 获得singleton对象，用于调用初始化函数
            auto sig = (Singleton *) lua_topointer(L, lua_upvalueindex(1));
            if (sig->init)
                sig->init();
        }
        return 1;
    }

    static int ImpFreeSingleton(lua_State *L) {
        lua_pushstring(L, "__shared");
        lua_rawget(L, 1);
        if (!lua_isnil(L, -1)) {
            // 释放
            lua_pushstring(L, "__shared");
            lua_pushnil(L);
            lua_rawset(L, 1);

            auto sig = (Singleton *) lua_topointer(L, lua_upvalueindex(1));
            if (sig->fini)
                sig->fini();
        }
        return 1;
    }

    Singleton singleton;
    Class::fields_type fields;
    Class::functions_type functions;
    Class::supers_type supers;
    Class::initfunctions_type inits;
    Class::function_type fini;
};

Class::Class() {
    NNT_CLASS_CONSTRUCT();
}

Class::~Class() {
    NNT_CLASS_DESTORY();
}

Class::fields_type const &Class::fields() const {
    return d_ptr->fields;
}

Class::functions_type const &Class::functions() const {
    return d_ptr->functions;
}

Class &Class::singleton(string const &_name, Singleton::func_type _init, Singleton::func_type _fini) {
    d_ptr->singleton.name = _name;
    d_ptr->singleton.init = ::std::move(_init);
    d_ptr->singleton.fini = ::std::move(_fini);
    return *this;
}

Class &Class::inherit(Any const &par) {
    d_ptr->supers.emplace_back(par);
    return *this;
}

Class &Class::inherit(::std::initializer_list<Any> const &pars) {
    for (auto &e : pars) {
        d_ptr->supers.emplace_back(e);
    }
    return *this;
}

Class &Class::init(Function::basefunc_type func, size_t args) {
    auto f = make_shared<Function>();
    ostringstream oss;
    oss << "__init" << args << "__";
    f->name = oss.str();
    f->classfunc = [=](self_type &self, args_type const &args) -> return_type {
        func(self, args);
        return nullptr;
    };
    d_ptr->inits.emplace_back(f);
    return *this;
}

Class &Class::init(Function::basefunc0_type func) {
    return init([=](self_type &self, args_type const &args) {
        return func(self);
    }, 0);
}

Class &Class::init(Function::basefunc1_type func) {
    return init([=](self_type &self, args_type const &args) {
        IMPFUNC_CHECK_ARGS(1);
        return func(self, COMXX_PPARGS_1(args));
    }, 1);
}

Class &Class::init(Function::basefunc2_type func) {
    return init([=](self_type &self, args_type const &args) {
        IMPFUNC_CHECK_ARGS(2);
        return func(self, COMXX_PPARGS_2(args));
    }, 2);
}

Class &Class::init(Function::basefunc3_type func) {
    return init([=](self_type &self, args_type const &args) {
        IMPFUNC_CHECK_ARGS(3);
        return func(self, COMXX_PPARGS_3(args));
    }, 3);
}

Class &Class::init(Function::basefunc4_type func) {
    return init([=](self_type &self, args_type const &args) {
        IMPFUNC_CHECK_ARGS(4);
        return func(self, COMXX_PPARGS_4(args));
    }, 4);
}

Class &Class::init(Function::basefunc5_type func) {
    return init([=](self_type &self, args_type const &args) {
        IMPFUNC_CHECK_ARGS(5);
        return func(self, COMXX_PPARGS_5(args));
    }, 5);
}

Class &Class::init(Function::basefunc6_type func) {
    return init([=](self_type &self, args_type const &args) {
        IMPFUNC_CHECK_ARGS(6);
        return func(self, COMXX_PPARGS_6(args));
    }, 6);
}

Class &Class::init(Function::basefunc7_type func) {
    return init([=](self_type &self, args_type const &args) {
        IMPFUNC_CHECK_ARGS(7);
        return func(self, COMXX_PPARGS_7(args));
    }, 7);
}

Class &Class::init(Function::basefunc8_type func) {
    return init([=](self_type &self, args_type const &args) {
        IMPFUNC_CHECK_ARGS(8);
        return func(self, COMXX_PPARGS_8(args));
    }, 8);
}

Class &Class::init(Function::basefunc9_type func) {
    return init([=](self_type &self, args_type const &args) {
        IMPFUNC_CHECK_ARGS(9);
        return func(self, COMXX_PPARGS_9(args));
    }, 9);
}

Class &Class::fini(Function::basefunc0_type func) {
    auto f = make_shared<Function>();
    f->name = "__fini__";
    f->classfunc = [=](self_type &self, args_type const &args) -> return_type {
        func(self);
        return nullptr;
    };
    d_ptr->fini = move(f);
    return *this;
}

Class &Class::add(string const &fname, Function::classfunc_type func) {
    auto f = make_shared<Function>();
    f->name = fname;
    f->classfunc = move(func);
    d_ptr->functions[f->name] = f;
    return *this;
}

Class &Class::add(string const &fname, Function::classfunc0_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        return func(self);
    });
}

Class &Class::add(string const &fname, Function::classfunc1_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(1);
        return func(self, COMXX_PPARGS_1(args));
    });
}

Class &Class::add(string const &fname, Function::classfunc2_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(2);
        return func(self, COMXX_PPARGS_2(args));
    });
}

Class &Class::add(string const &fname, Function::classfunc3_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(3);
        return func(self, COMXX_PPARGS_3(args));
    });
}

Class &Class::add(string const &fname, Function::classfunc4_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(4);
        return func(self, COMXX_PPARGS_4(args));
    });
}

Class &Class::add(string const &fname, Function::classfunc5_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(5);
        return func(self, COMXX_PPARGS_5(args));
    });
}

Class &Class::add(string const &fname, Function::classfunc6_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(6);
        return func(self, COMXX_PPARGS_6(args));
    });
}

Class &Class::add(string const &fname, Function::classfunc7_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(7);
        return func(self, COMXX_PPARGS_7(args));
    });
}

Class &Class::add(string const &fname, Function::classfunc8_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(8);
        return func(self, COMXX_PPARGS_8(args));
    });
}

Class &Class::add(string const &fname, Function::classfunc9_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(9);
        return func(self, COMXX_PPARGS_9(args));
    });
}

Class &Class::add_static(string const &fname, Function::func_type func) {
    auto f = make_shared<Function>();
    f->name = fname;
    f->func = move(func);
    d_ptr->functions[f->name] = f;
    return *this;
}

Class &Class::add_static(string const &fname, Function::func0_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        return func();
    });
}

Class &Class::add_static(string const &fname, Function::func1_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(1);
        return func(COMXX_PPARGS_1(args));
    });
}

Class &Class::add_static(string const &fname, Function::func2_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(2);
        return func(COMXX_PPARGS_2(args));
    });
}

Class &Class::add_static(string const &fname, Function::func3_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(3);
        return func(COMXX_PPARGS_3(args));
    });
}

Class &Class::add_static(string const &fname, Function::func4_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(4);
        return func(COMXX_PPARGS_4(args));
    });
}

Class &Class::add_static(string const &fname, Function::func5_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(5);
        return func(COMXX_PPARGS_5(args));
    });
}

Class &Class::add_static(string const &fname, Function::func6_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(6);
        return func(COMXX_PPARGS_6(args));
    });
}

Class &Class::add_static(string const &fname, Function::func7_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(7);
        return func(COMXX_PPARGS_7(args));
    });
}

Class &Class::add_static(string const &fname, Function::func8_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(8);
        return func(COMXX_PPARGS_8(args));
    });
}

Class &Class::add_static(string const &fname, Function::func9_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(9);
        return func(COMXX_PPARGS_9(args));
    });
}

Class &Class::add(field_type const &f) {
    d_ptr->fields[f->name] = f;
    return *this;
}

void Singleton::declare_in(Context &ctx) const {
    auto L = GetContextL(ctx);
    NLUA_AUTOSTACK(L);

    int clzid = lua_gettop(L);

    lua_pushstring(L, name.c_str());
    lua_pushlightuserdata(L, (void *) this);
    lua_pushcclosure(L, ClassPrivate::ImpGetSingleton, 1);
    lua_rawset(L, clzid);

    lua_pushstring(L, ("free_" + name).c_str());
    lua_pushlightuserdata(L, (void *) this);
    lua_pushcclosure(L, ClassPrivate::ImpFreeSingleton, 1);
    lua_rawset(L, clzid);

    lua_pushstring(L, "__shared");
    lua_pushnil(L);
    lua_rawset(L, clzid);

    lua_pushstring(L, "__singleton_new__");
    lua_pushcfunction(L, ClassPrivate::ImpNew);
    lua_rawset(L, clzid);
}

void Class::declare_in(Context &ctx) const {
    auto L = GetContextL(ctx);
    NLUA_AUTOSTACK(L);

    lua_newtable(L);
    int clzid = lua_gettop(L);

    if (d_ptr->singleton.empty()) {
        lua_pushstring(L, "new");
        lua_pushcfunction(L, ClassPrivate::ImpNew);
        lua_rawset(L, clzid);
    } else {
        d_ptr->singleton.declare_in(ctx);
    }

    // 定义析构函数
    if (d_ptr->fini) {
        lua_pushstring(L, "delete");
        lua_pushcfunction(L, ClassPrivate::ImpDestroy);
        lua_rawset(L, clzid);
    }

    // 类对象放到栈顶
    lua_pushvalue(L, clzid);
    d_ptr->body_declare_in(ctx, *this);

    // 定义到全局
    lua_pushvalue(L, clzid);
    lua_setglobal(L, name.c_str());
};

void Class::declare_in(Context &ctx, Module const &mod) const {
    auto L = GetContextL(ctx);
    NLUA_AUTOSTACK(L);

    lua_newtable(L);
    int clzid = lua_gettop(L);

    if (d_ptr->singleton.empty()) {
        lua_pushstring(L, "new");
        lua_pushcfunction(L, ClassPrivate::ImpNew);
        lua_rawset(L, clzid);
    } else {
        d_ptr->singleton.declare_in(ctx);
    }

    // 定义析构函数
    if (d_ptr->fini) {
        lua_pushstring(L, "delete");
        lua_pushcfunction(L, ClassPrivate::ImpDestroy);
        lua_rawset(L, clzid);
    }

    // 类对象放到栈顶
    lua_pushvalue(L, clzid);
    d_ptr->body_declare_in(ctx, *this);

    // 定义到module中
    lua_getglobal(L, mod.name.c_str());
    int modid = lua_gettop(L);
    lua_pushstring(L, name.c_str());
    lua_pushvalue(L, clzid);
    lua_rawset(L, modid);
}

class ModulePrivate {
public:
    Module *d_owner;
    Module::classes_type classes;

    void body_declare_in(Context &ctx) {
        for (auto &e : classes) {
            e.second->declare_in(ctx, *d_owner);
        }
    }
};

Module::Module() {
    NNT_CLASS_CONSTRUCT();
    d_ptr->d_owner = this;
}

Module::~Module() {
    d_ptr->d_owner = nullptr;
    NNT_CLASS_DESTORY()
}

Module &Module::add(class_type &c) {
    d_ptr->classes[c->name] = c;
    return *this;
}

void Module::declare_in(Context &ctx) const {
    auto L = GetContextL(ctx);
    NLUA_AUTOSTACK(L);

    // 判断是否已经被定义, module不允许覆盖
    lua_getglobal(L, name.c_str());
    if (!lua_isnil(L, -1)) {
        d_ptr->body_declare_in(ctx);
        return;
    }

    lua_newtable(L);
    int modid = lua_gettop(L);

    lua_pushstring(L, "__index");
    lua_pushvalue(L, modid);
    lua_rawset(L, modid);
    lua_setglobal(L, name.c_str());

    d_ptr->body_declare_in(ctx);
}

void Module::merge(Module const &r) {
    for (auto &e : r.d_ptr->classes) {
        d_ptr->classes[e.first] = e.second;
    }
}

Object::Object() {
    NNT_CLASS_CONSTRUCT()
}

Object::~Object() {
    NNT_CLASS_DESTORY()
}

void *Object::payload() const {
    auto L = d_ptr->L;
    NLUA_AUTOSTACK(L);

    if (d_ptr->id) {
        lua_pushvalue(L, d_ptr->id);
    } else {
        lua_getglobal(L, d_ptr->name.c_str());
    }

    if (!lua_isuserdata(L, -1))
        return nullptr;

    auto ud = (ObjectPrivate::UserData *) lua_touserdata(L, -1);
    return ud->data;
}

void Object::payload(void *data) {
    auto L = d_ptr->L;
    NLUA_AUTOSTACK(L);

    if (d_ptr->id) {
        lua_pushvalue(L, d_ptr->id);
    } else {
        lua_getglobal(L, d_ptr->name.c_str());
    }

    auto ud = (ObjectPrivate::UserData *) lua_touserdata(L, -1);
    ud->data = data;
}

return_type Object::get(string const &name) {
    auto L = d_ptr->L;
    NLUA_AUTOSTACK(L);

    if (d_ptr->id) {
        lua_pushvalue(L, d_ptr->id);
    } else {
        lua_getglobal(L, d_ptr->name.c_str());
    }

    lua_pushstring(L, name.c_str());
    lua_rawget(L, -2);

    return at(L, -1);
}

void Object::set(string const &name, value_type const &val) {
    auto L = d_ptr->L;
    NLUA_AUTOSTACK(L);

    if (d_ptr->id) {
        // 局部变量
        lua_pushvalue(L, d_ptr->id);
    } else {
        // 全局变量
        lua_getglobal(L, d_ptr->name.c_str());
    }

    lua_pushstring(L, name.c_str());
    push(val.get(), L);

    lua_rawset(L, -3);
}

self_type Context::global(string const &name) {
    auto L = d_ptr->L;
    NLUA_AUTOSTACK(L);

    lua_getglobal(L, name.c_str());
    if (lua_isnil(L, -1))
        return nullptr;

    self_type r = make_shared<Object>();
    r->d_ptr->name = name;
    r->d_ptr->L = L;
    return r;
}

void Object::setfor(Object &r) const {
    auto L = d_ptr->L;
    NLUA_AUTOSTACK(L);

    if (r.d_ptr->id) {
        lua_pushvalue(L, r.d_ptr->id);
    } else {
        lua_getglobal(L, r.d_ptr->name.c_str());
    }
    int toid = lua_gettop(L);

    if (d_ptr->id) {
        lua_pushvalue(L, d_ptr->id);
    } else {
        lua_getglobal(L, d_ptr->name.c_str());
    }
    int fromid = lua_gettop(L);

    // 从from复制到to
    lua_pushnil(L); // first key
    while (lua_next(L, fromid)) {
        // key -2 value -1
        // char const *k = lua_typename(L, lua_type(L, -2));
        // char const *v = lua_typename(L, lua_type(L, -1));
        // cout << "key: " << k << " " << v << endl;

        // dup key并添加到-2处，避免rawset后，key被pop掉
        lua_pushvalue(L, -2);
        lua_insert(L, -2);

        // 设置到目标类
        lua_rawset(L, toid);

        // remove value, keep key for next iterator
        // lua_pop(L, 1);
    }
}

bool Object::has(string const &name) const {
    auto L = d_ptr->L;
    NLUA_AUTOSTACK(L);

    if (d_ptr->id) {
        // 是局部变量
        lua_getfield(L, d_ptr->id, name.c_str());
        if (lua_isnil(L, -1))
            return false;
    } else {
        // 获得全局变量
        lua_getglobal(L, d_ptr->name.c_str());
        if (lua_isnil(L, -1))
            return false;

        // 获得的函数
        lua_getfield(L, -1, name.c_str());
        if (lua_isnil(L, -1))
            return false;
    }

    return true;
}

bool Object::isnull() const {
    return !d_ptr->L || !d_ptr->id || d_ptr->name.empty();
}

void Object::grab() {
    auto L = d_ptr->L;
    NLUA_AUTOSTACK(L);

    if (d_ptr->id) {
        // 设置生命期到全局，避免被局部释放
        ostringstream oss;
        oss << "__nluaxx_global_objects_" << ObjectPrivate::RefId.fetch_add(1);
        d_ptr->name = oss.str();

        lua_pushvalue(L, d_ptr->id);
        lua_setglobal(L, d_ptr->name.c_str());
        d_ptr->id = 0;
    }

    // 已经是全局变量
    lua_getglobal(L, d_ptr->name.c_str());
    if (!lua_istable(L, -1)) {
        cerr << "当前变量不是对象" << d_ptr->name << endl;
        return;
    }
    int selfid = lua_gettop(L);
    lua_getfield(L, selfid, "__refs");
    if (lua_isnumber(L, -1)) {
        lua_pushinteger(L, lua_tointeger(L, -1) + 1);
    } else {
        lua_pushinteger(L, 1);
    }
    lua_setfield(L, selfid, "__refs");
}

bool Object::drop() {
    auto L = d_ptr->L;
    NLUA_AUTOSTACK(L);

    if (d_ptr->id) {
        cerr << "该变量是局部变量，不支持drop" << endl;
        return false;
    }

    lua_getglobal(L, d_ptr->name.c_str());
    if (lua_isnil(L, -1)) {
        cerr << "没有找到变量" << d_ptr->name << endl;
        return false;
    }
    int selfid = lua_gettop(L);
    lua_getfield(L, selfid, "__refs");
    if (!lua_isnumber(L, -1)) {
        cerr << "该变量没有进行过grab操作" << endl;
        return false;
    }

    auto refs = lua_tointeger(L, -1);

    if (--refs == 0) {
        // 需要释放
        lua_pushnil(L);
        lua_setglobal(L, d_ptr->name.c_str());
        return true;
    }

    lua_pushinteger(L, refs);
    lua_setfield(L, selfid, "__refs");
    return false;
}

return_type Object::invoke(string const &name, args_type const &args) {
    auto L = d_ptr->L;
    NLUA_AUTOSTACK(L);

    lua_pushcfunction(L, ContextPrivate::Traceback);
    int tbid = lua_gettop(L);

    if (d_ptr->id) {
        // 是局部变量
        lua_getfield(L, d_ptr->id, name.c_str());
        if (lua_isnil(L, -1)) {
            cerr << "没有找到函数" << name << endl;
            return nullptr;
        }
        if (!lua_isfunction(L, -1)) {
            // cerr << name << "当前不是函数" << endl;
            return nullptr;
        }

        // push self
        lua_pushvalue(L, d_ptr->id);
    } else {
        // 获得全局变量
        lua_getglobal(L, d_ptr->name.c_str());
        if (lua_isnil(L, -1)) {
            cerr << "没有找到变量" << d_ptr->name << endl;
            return nullptr;
        }
        int selfid = lua_gettop(L);

        // 获得的函数
        lua_getfield(L, -1, name.c_str());
        if (lua_isnil(L, -1)) {
            cerr << "没有找到函数" << name << endl;
            return nullptr;
        }
        if (!lua_isfunction(L, -1)) {
            // cerr << name << "当前不是函数" << endl;
            return nullptr;
        }

        // push self
        lua_pushvalue(L, selfid);
    }

    for (auto &e : args) {
        push(e, L);
    }

    int s = lua_pcall(L, args.size() + 1, 1, tbid);
    if (LUA_OK != s)
        return nullptr;
    return at(L, -1);
}

return_type Object::invoke(string const &name) {
    return invoke(name, {});
}

return_type Object::invoke(string const &name, Variant const &v0) {
    return invoke(name, {v0});
}

return_type Object::invoke(string const &name, Variant const &v0, Variant const &v1) {
    return invoke(name, {v0, v1});
}

return_type Object::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2) {
    return invoke(name, {v0, v1, v2});
}

return_type Object::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2, Variant const &v3) {
    return invoke(name, {v0, v1, v2, v3});
}

return_type Object::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2, Variant const &v3, Variant const &v4) {
    return invoke(name, {v0, v1, v2, v3, v4});
}

return_type Object::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2, Variant const &v3, Variant const &v4, Variant const &v5) {
    return invoke(name, {v0, v1, v2, v3, v4, v5});
}

return_type Object::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2, Variant const &v3, Variant const &v4, Variant const &v5, Variant const &v6) {
    return invoke(name, {v0, v1, v2, v3, v4, v5, v6});
}

return_type Object::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2, Variant const &v3, Variant const &v4, Variant const &v5, Variant const &v6, Variant const &v7) {
    return invoke(name, {v0, v1, v2, v3, v4, v5, v6, v7});
}

return_type Object::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2, Variant const &v3, Variant const &v4, Variant const &v5, Variant const &v6, Variant const &v7, Variant const &v8) {
    return invoke(name, {v0, v1, v2, v3, v4, v5, v6, v7, v8});
}

return_type Object::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2, Variant const &v3, Variant const &v4, Variant const &v5, Variant const &v6, Variant const &v7, Variant const &v8, Variant const &v9) {
    return invoke(name, {v0, v1, v2, v3, v4, v5, v6, v7, v8, v9});
}

NLUA_END
