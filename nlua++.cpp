#include "core.h"
#include "nlua++.h"
#include NLUA_INCLUDE(lua.hpp)
#include "value.h"
#include <atomic>

NLUA_BEGIN

#define IMPFUNC_CHECK_ARGS(count) do { if (args.size() < count) throw error(-1, "缺少参数"); } while(0);

struct ContextPrivate {
    ContextPrivate() {
        L = luaL_newstate();
        luaL_openlibs(L);
    }

    ~ContextPrivate() {
        lua_close(L);
        L = nullptr;
    }

    bool loadfile(path const &file) {
        int s = luaL_dofile(L, file.c_str());
        return LUA_OK == s;
    }

    static int Traceback(lua_State *L) {
        if (!lua_isstring(L, 1))
            return 1;
        cerr << "捕获错误 " << luaL_checkstring(L, 1) << endl;
        return 0;
    }

    lua_State *L;
};

static lua_State *GetContextL(Context &ctx) {
    return DPtr < Context, ContextPrivate > (&ctx)->L;
}

Context::Context() {
    NLUA_CLASS_CONSTRUCT()

    libraries_path = path::getcwd();
}

Context::~Context() {
    NLUA_CLASS_DESTORY()
}

bool Context::load(const path &file) {
    path tgt = file;
    if (!path::exists(tgt)) {
        tgt = path::absolute(libraries_path) + "/" + file;
        if (!path::exists(tgt)) {
            cerr << "没有找到 " << file << endl;
            return false;
        }
    }

    if (path::isdirectory(tgt)) {
        auto walk = path::walk(tgt);
        for (auto &i:walk) {
            path cur = tgt + "/" + i;
            if (!load(cur)) {
                cerr << "加载失败 " << cur << endl;
                continue;;
            }
        }
        return true;
    }

    return d_ptr->loadfile(tgt);
}

return_type Context::invoke(string const &fname, args_type const &args) {
    auto L = d_ptr->L;
    NLUA_AUTOSTACK(L);

    lua_pushcfunction(L, ContextPrivate::Traceback);
    int tbid = lua_gettop(L);

    lua_getglobal(L, fname.c_str());

    for (auto &e:args) {
        push(e, L);
    }

    int s = lua_pcall(L, args.size(), 1, tbid);
    if (LUA_OK != s)
        return nullptr;
    return at(L, -1);
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

struct ObjectPrivate {
    // 对象内存指针
    void *self;

    // 当前栈
    lua_State *L;

    // 当前栈上对象的id（local对象）
    int id = 0;

    // 对象名
    string name;
};

struct FunctionPrivate {

    FunctionPrivate() {
        id = RefId.fetch_add(1);
    }

    // 生成的函数唯一id
    ulonglong id;

    typedef ::std::function<return_type(lua_State *L, self_type &self, args_type const &)> classfunc_type;
    typedef ::std::function<return_type(lua_State *L, args_type const &)> func_type;

    typedef ::std::function<int(lua_State *)> lua_cfunction_type;
    typedef ::std::atomic<ulonglong> lua_cfunction_refid_type;

    typedef ::std::map<ulonglong, classfunc_type> lua_ref_classfuncs_type;
    typedef ::std::map<ulonglong, func_type> lua_ref_funcs_type;

    static lua_cfunction_refid_type RefId;
    static lua_ref_classfuncs_type ClassFuncs;
    static lua_ref_funcs_type Funcs;

    static int ImpClassFunction(lua_State *L) {
        ulonglong id = lua_tointeger(L, lua_upvalueindex(1));
        auto fnd = ClassFuncs.find(id);
        if (fnd == ClassFuncs.end()) {
            cerr << "没有找到对应的回调函数" << endl;
            return 0;
        }

        return_type rv;
        self_type self = make_shared<Object>();
        self->d_ptr->id = lua_gettop(L);
        self->d_ptr->self = (void *) lua_topointer(L, -1);
        self->d_ptr->L = L;

        int count = lua_gettop(L) - 1;
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
        ulonglong id = lua_tointeger(L, lua_upvalueindex(1));
        auto fnd = Funcs.find(id);
        if (fnd == Funcs.end()) {
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

FunctionPrivate::lua_cfunction_refid_type FunctionPrivate::RefId(1);
FunctionPrivate::lua_ref_classfuncs_type FunctionPrivate::ClassFuncs;
FunctionPrivate::lua_ref_funcs_type FunctionPrivate::Funcs;

Function::Function() {
    NLUA_CLASS_CONSTRUCT()
}

Function::~Function() {
    NLUA_CLASS_DESTORY()
}

void Function::declare_in(Context &ctx) const {
    auto L = GetContextL(ctx);
    NLUA_AUTOSTACK(L);

    // 注册到全局对照表中，用于激活函数时查找真正的执行函数
    private_class_type::Funcs.insert(make_pair(d_ptr->id, [=](lua_State *L, args_type const &args) -> return_type {
        try {
            return this->func(args);
        } catch (exception &e) {
            luaL_error(L, e.what());
        }
        return nullptr;
    }));

    lua_pushinteger(L, d_ptr->id);
    lua_pushcclosure(L, private_class_type::ImpStaticFunction, 1);

    lua_setglobal(L, name.c_str());
}

void Function::declare_in(Context &ctx, Class const &clz) const {
    auto L = GetContextL(ctx);
    NLUA_AUTOSTACK(L);

    // 绑定到类
    int clzid = lua_gettop(L);
    lua_pushstring(L, name.c_str());
    lua_pushinteger(L, d_ptr->id);

    if (classfunc) {
        // 注册到全局对照表中，用于激活函数时查找真正的执行函数
        private_class_type::ClassFuncs.insert(make_pair(d_ptr->id, [=](lua_State *L, self_type &self, args_type const &args) -> return_type {
            try {
                return this->classfunc(self, args);
            } catch (exception &e) {
                luaL_error(L, e.what());
            }
            return nullptr;
        }));
        lua_pushcclosure(L, private_class_type::ImpClassFunction, 1);
    } else {
        // 注册静态函数
        private_class_type::Funcs.insert(make_pair(d_ptr->id, [=](lua_State *L, args_type const &args) -> return_type {
            try {
                return this->func(args);
            } catch (exception &e) {
                luaL_error(L, e.what());
            }
            return nullptr;
        }));
        lua_pushcclosure(L, private_class_type::ImpStaticFunction, 1);
    }

    lua_rawset(L, clzid);
}

struct ClassPrivate {
    Class *d_owner;

    // 仅输出定义段
    void body_declare_in(Context &ctx, Class const &_curclass) {
        auto L = GetContextL(ctx);
        NLUA_AUTOSTACK(L);

        for (auto &e:d_owner->_supers) {
            if (e.clazz) {
                e.clazz->d_ptr->body_declare_in(ctx, _curclass);
            } else if (e.object) {
                Object t; // 获得当前类对象，将父类进行keyvalue设置
                t.d_ptr->id = lua_gettop(L);
                e.object->setfor(t);
            }
        }

        for (auto &e:d_owner->_functions) {
            e.second->declare_in(ctx, _curclass);
        }

        for (auto &e:d_owner->_fields) {
            e.second->declare_in(ctx, _curclass);
        }
    }

    // 实例化类型
    static int ImpNew(lua_State *L) {
        // NLUA_AUTOSTACK(L); 会导致实例化的栈被弹出
        int clzid = lua_gettop(L);

        lua_newtable(L);
        int metaclzid = lua_gettop(L);

        // instance
        lua_newtable(L);
        int objid = lua_gettop(L);

        // {__index=x}
        lua_pushstring(L, "__index");
        lua_pushvalue(L, clzid);
        lua_rawset(L, objid);

        lua_setmetatable(L, metaclzid);
        return 1;
    }

    // 实例化
    static void CallNew(lua_State *L) {
        // 1 clzid
        lua_pushcfunction(L, ContextPrivate::Traceback);
        // 2 tbid
        lua_pushstring(L, "__new__");
        lua_rawget(L, 1);
        lua_pushvalue(L, 1);
        int s = lua_pcall(L, 1, 1, 2);
        if (s != LUA_OK) {
            lua_pushnil(L);
        }
    }

    // 单件实现
    static int ImpGetSingleton(lua_State *L) {
        lua_pushstring(L, "__shared");
        lua_rawget(L, 1);
        if (lua_isnil(L, -1)) {
            // 恢复堆栈
            lua_pop(L, 1);
            // 调用new函数
            CallNew(L);
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
};

Class::Class() {
    NLUA_CLASS_CONSTRUCT()
    d_ptr->d_owner = this;
}

Class::~Class() {
    d_ptr->d_owner = nullptr;
    NLUA_CLASS_DESTORY()
}

Class &Class::add(string const &fname, Function::classfunc_type func) {
    auto f = make_shared<Function>();
    f->name = fname;
    f->classfunc = ::std::move(func);
    _functions.insert(make_pair(f->name, f));
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
        auto const &v0 = args.begin() + 0;
        return func(self, v0);
    });
}

Class &Class::add(string const &fname, Function::classfunc2_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(2);
        auto const &v0 = args.begin() + 0;
        auto const &v1 = args.begin() + 1;
        return func(self, v0, v1);
    });
}

Class &Class::add(string const &fname, Function::classfunc3_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(3);
        auto const &v0 = args.begin() + 0;
        auto const &v1 = args.begin() + 1;
        auto const &v2 = args.begin() + 2;
        return func(self, v0, v1, v2);
    });
}

Class &Class::add(string const &fname, Function::classfunc4_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(4);
        auto const &v0 = args.begin() + 0;
        auto const &v1 = args.begin() + 1;
        auto const &v2 = args.begin() + 2;
        auto const &v3 = args.begin() + 3;
        return func(self, v0, v1, v2, v3);
    });
}

Class &Class::add(string const &fname, Function::classfunc5_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(5);
        auto const &v0 = args.begin() + 0;
        auto const &v1 = args.begin() + 1;
        auto const &v2 = args.begin() + 2;
        auto const &v3 = args.begin() + 3;
        auto const &v4 = args.begin() + 4;
        return func(self, v0, v1, v2, v3, v4);
    });
}

Class &Class::add(string const &fname, Function::classfunc6_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(6);
        auto const &v0 = args.begin() + 0;
        auto const &v1 = args.begin() + 1;
        auto const &v2 = args.begin() + 2;
        auto const &v3 = args.begin() + 3;
        auto const &v4 = args.begin() + 4;
        auto const &v5 = args.begin() + 5;
        return func(self, v0, v1, v2, v3, v4, v5);
    });
}

Class &Class::add(string const &fname, Function::classfunc7_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(7);
        auto const &v0 = args.begin() + 0;
        auto const &v1 = args.begin() + 1;
        auto const &v2 = args.begin() + 2;
        auto const &v3 = args.begin() + 3;
        auto const &v4 = args.begin() + 4;
        auto const &v5 = args.begin() + 5;
        auto const &v6 = args.begin() + 6;
        return func(self, v0, v1, v2, v3, v4, v5, v6);
    });
}

Class &Class::add(string const &fname, Function::classfunc8_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(8);
        auto const &v0 = args.begin() + 0;
        auto const &v1 = args.begin() + 1;
        auto const &v2 = args.begin() + 2;
        auto const &v3 = args.begin() + 3;
        auto const &v4 = args.begin() + 4;
        auto const &v5 = args.begin() + 5;
        auto const &v6 = args.begin() + 6;
        auto const &v7 = args.begin() + 7;
        return func(self, v0, v1, v2, v3, v4, v5, v6, v7);
    });
}

Class &Class::add(string const &fname, Function::classfunc9_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(9);
        auto const &v0 = args.begin() + 0;
        auto const &v1 = args.begin() + 1;
        auto const &v2 = args.begin() + 2;
        auto const &v3 = args.begin() + 3;
        auto const &v4 = args.begin() + 4;
        auto const &v5 = args.begin() + 5;
        auto const &v6 = args.begin() + 6;
        auto const &v7 = args.begin() + 7;
        auto const &v8 = args.begin() + 8;
        return func(self, v0, v1, v2, v3, v4, v5, v6, v7, v8);
    });
}

Class &Class::add_static(string const &fname, Function::func_type func) {
    auto f = make_shared<Function>();
    f->name = fname;
    f->func = ::std::move(func);
    _functions.insert(make_pair(f->name, f));
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
        auto const &v0 = args.begin() + 0;
        return func(v0);
    });
}

Class &Class::add_static(string const &fname, Function::func2_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(2);
        auto const &v0 = args.begin() + 0;
        auto const &v1 = args.begin() + 1;
        return func(v0, v1);
    });
}

Class &Class::add_static(string const &fname, Function::func3_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(3);
        auto const &v0 = args.begin() + 0;
        auto const &v1 = args.begin() + 1;
        auto const &v2 = args.begin() + 2;
        return func(v0, v1, v2);
    });
}

Class &Class::add_static(string const &fname, Function::func4_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(4);
        auto const &v0 = args.begin() + 0;
        auto const &v1 = args.begin() + 1;
        auto const &v2 = args.begin() + 2;
        auto const &v3 = args.begin() + 3;
        return func(v0, v1, v2, v3);
    });
}

Class &Class::add_static(string const &fname, Function::func5_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(5);
        auto const &v0 = args.begin() + 0;
        auto const &v1 = args.begin() + 1;
        auto const &v2 = args.begin() + 2;
        auto const &v3 = args.begin() + 3;
        auto const &v4 = args.begin() + 4;
        return func(v0, v1, v2, v3, v4);
    });
}

Class &Class::add_static(string const &fname, Function::func6_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(6);
        auto const &v0 = args.begin() + 0;
        auto const &v1 = args.begin() + 1;
        auto const &v2 = args.begin() + 2;
        auto const &v3 = args.begin() + 3;
        auto const &v4 = args.begin() + 4;
        auto const &v5 = args.begin() + 5;
        return func(v0, v1, v2, v3, v4, v5);
    });
}

Class &Class::add_static(string const &fname, Function::func7_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(7);
        auto const &v0 = args.begin() + 0;
        auto const &v1 = args.begin() + 1;
        auto const &v2 = args.begin() + 2;
        auto const &v3 = args.begin() + 3;
        auto const &v4 = args.begin() + 4;
        auto const &v5 = args.begin() + 5;
        auto const &v6 = args.begin() + 6;
        return func(v0, v1, v2, v3, v4, v5, v6);
    });
}

Class &Class::add_static(string const &fname, Function::func8_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(8);
        auto const &v0 = args.begin() + 0;
        auto const &v1 = args.begin() + 1;
        auto const &v2 = args.begin() + 2;
        auto const &v3 = args.begin() + 3;
        auto const &v4 = args.begin() + 4;
        auto const &v5 = args.begin() + 5;
        auto const &v6 = args.begin() + 6;
        auto const &v7 = args.begin() + 7;
        return func(v0, v1, v2, v3, v4, v5, v6, v7);
    });
}

Class &Class::add_static(string const &fname, Function::func9_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(9);
        auto const &v0 = args.begin() + 0;
        auto const &v1 = args.begin() + 1;
        auto const &v2 = args.begin() + 2;
        auto const &v3 = args.begin() + 3;
        auto const &v4 = args.begin() + 4;
        auto const &v5 = args.begin() + 5;
        auto const &v6 = args.begin() + 6;
        auto const &v7 = args.begin() + 7;
        auto const &v8 = args.begin() + 8;
        return func(v0, v1, v2, v3, v4, v5, v6, v7, v8);
    });
}

Class &Class::add(field_type const &f) {
    if (_fields.find(f->name) == _fields.end()) {
        _fields.insert(make_pair(f->name, f));
    }
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

    lua_pushstring(L, "__new__");
    lua_pushcfunction(L, ClassPrivate::ImpNew);
    lua_rawset(L, clzid);
}

void Class::declare_in(Context &ctx) const {
    auto L = GetContextL(ctx);
    NLUA_AUTOSTACK(L);

    lua_newtable(L);
    int clzid = lua_gettop(L);

    if (_singleton.empty()) {
        lua_pushstring(L, "new");
        lua_pushcfunction(L, ClassPrivate::ImpNew);
        lua_rawset(L, clzid);
    } else {
        _singleton.declare_in(ctx);
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

    if (_singleton.empty()) {
        lua_pushstring(L, "new");
        lua_pushcfunction(L, ClassPrivate::ImpNew);
        lua_rawset(L, clzid);
    } else {
        _singleton.declare_in(ctx);
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

Module &Module::add_class(class_type &c) {
    _classes.insert(make_pair(c->name, c));
    return *this;
}

void Module::declare_in(Context &ctx) const {
    auto L = GetContextL(ctx);
    NLUA_AUTOSTACK(L);

    lua_newtable(L);
    int modid = lua_gettop(L);

    lua_pushstring(L, "__index");
    lua_pushvalue(L, modid);
    lua_rawset(L, modid);
    lua_setglobal(L, name.c_str());

    for (auto &e:_classes) {
        e.second->declare_in(ctx, *this);
    }
};

Object::Object() {
    NLUA_CLASS_CONSTRUCT()
}

Object::~Object() {
    NLUA_CLASS_DESTORY()
}

void *Object::pointer() const {
    return (void *) d_ptr->self;
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
    r->d_ptr->self = (void *) lua_topointer(L, -1);
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

return_type Object::invoke(string const &name, args_type const &args) {
    auto L = d_ptr->L;
    NLUA_AUTOSTACK(L);

    lua_pushcfunction(L, ContextPrivate::Traceback);
    int tbid = lua_gettop(L);

    if (d_ptr->id) {
        // 是局部变量
        lua_getfield(L, d_ptr->id, name.c_str());
        if (lua_isnil(L, -1))
            return nullptr;
        lua_pushvalue(L, d_ptr->id);
    } else {
        // 获得全局变量
        lua_getglobal(L, d_ptr->name.c_str());
        if (lua_isnil(L, -1))
            return nullptr;
        int selfid = lua_gettop(L);

        // 获得的函数
        lua_getfield(L, -1, name.c_str());
        if (lua_isnil(L, -1))
            return nullptr;

        // push self
        lua_pushvalue(L, selfid);
    }

    for (auto &e:args) {
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
