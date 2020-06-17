#include "nlua++.hpp"
#define __NLUA_PRIVATE__
#include "ast.hpp"
#include "liblua.hpp"
#include "value.hpp"
#include "private.hpp"
#include <atomic>
#include <sstream>

#include <cross/cross.hpp>
#include <cross/str.hpp>

NLUA_BEGIN

USE_STL;
USE_CROSS;

void Field::declare_in(Context &ctx) const {
    auto L = ctx.d().L;
    NLUA_AUTOSTACK(L);

    push(val.get(), L);
    lua_setglobal(L, name.c_str());
}

void Field::declare_in(Context &ctx, Class const &clz) const {
    auto L = ctx.d().L;
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

    // grab后的栈
    lua_State *GL = nullptr;

    // 当前栈上对象的id（local对象）
    int id = 0;

    // 对象名
    string name;

    typedef ::std::atomic<lua_Integer> lua_global_refid_type;
    static lua_global_refid_type RefId;
};

ObjectPrivate::lua_global_refid_type ObjectPrivate::RefId(1);

class FunctionPrivate {
public:

    static int ImpClassFunction(lua_State *L) {
        auto pctx = (ContextPrivate *)lua_topointer(L, lua_upvalueindex(1));
        auto fnid = lua_tointeger(L, lua_upvalueindex(2));

        auto fnd = pctx->refClassFuncs.find(fnid);
        if (fnd == pctx->refClassFuncs.end()) {
            NLUA_ERROR("没有找到对应的回调函数");
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
            rv = fnd->second(L, self, { v0.get() });
        }
                break;
        case 2: {
            auto v0 = at(L, 2);
            auto v1 = at(L, 3);
            rv = fnd->second(L, self, { v0.get(), v1.get() });
        }
                break;
        case 3: {
            auto v0 = at(L, 2);
            auto v1 = at(L, 3);
            auto v2 = at(L, 4);
            rv = fnd->second(L, self, { v0.get(), v1.get(), v2.get() });
        }
                break;
        case 4: {
            auto v0 = at(L, 2);
            auto v1 = at(L, 3);
            auto v2 = at(L, 4);
            auto v3 = at(L, 5);
            rv = fnd->second(L, self, { v0.get(), v1.get(), v2.get(), v3.get() });
        }
                break;
        case 5: {
            auto v0 = at(L, 2);
            auto v1 = at(L, 3);
            auto v2 = at(L, 4);
            auto v3 = at(L, 5);
            auto v4 = at(L, 6);
            rv = fnd->second(L, self, { v0.get(), v1.get(), v2.get(), v3.get(), v4.get() });
        }
                break;
        case 6: {
            auto v0 = at(L, 2);
            auto v1 = at(L, 3);
            auto v2 = at(L, 4);
            auto v3 = at(L, 5);
            auto v4 = at(L, 6);
            auto v5 = at(L, 7);
            rv = fnd->second(L, self, { v0.get(), v1.get(), v2.get(), v3.get(), v4.get(), v5.get() });
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
            rv = fnd->second(L, self, { v0.get(), v1.get(), v2.get(), v3.get(), v4.get(), v5.get(), v6.get() });
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
            rv = fnd->second(L, self, { v0.get(), v1.get(), v2.get(), v3.get(), v4.get(), v5.get(), v6.get(), v7.get() });
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
            rv = fnd->second(L, self, { v0.get(), v1.get(), v2.get(), v3.get(), v4.get(), v5.get(), v6.get(), v7.get(), v8.get() });
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
        auto pctx = (ContextPrivate *)lua_topointer(L, lua_upvalueindex(1));
        auto fnid = lua_tointeger(L, lua_upvalueindex(2));

        auto fnd = pctx->refFuncs.find(fnid);
        if (fnd == pctx->refFuncs.end()) {
            NLUA_ERROR("没有找到对应的回调函数");
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
            rv = fnd->second(L, { v0.get() });
        }
                break;
        case 2: {
            auto v0 = at(L, 2);
            auto v1 = at(L, 3);
            rv = fnd->second(L, { v0.get(), v1.get() });
        }
                break;
        case 3: {
            auto v0 = at(L, 2);
            auto v1 = at(L, 3);
            auto v2 = at(L, 4);
            rv = fnd->second(L, { v0.get(), v1.get(), v2.get() });
        }
                break;
        case 4: {
            auto v0 = at(L, 2);
            auto v1 = at(L, 3);
            auto v2 = at(L, 4);
            auto v3 = at(L, 5);
            rv = fnd->second(L, { v0.get(), v1.get(), v2.get(), v3.get() });
        }
                break;
        case 5: {
            auto v0 = at(L, 2);
            auto v1 = at(L, 3);
            auto v2 = at(L, 4);
            auto v3 = at(L, 5);
            auto v4 = at(L, 6);
            rv = fnd->second(L, { v0.get(), v1.get(), v2.get(), v3.get(), v4.get() });
        }
                break;
        case 6: {
            auto v0 = at(L, 2);
            auto v1 = at(L, 3);
            auto v2 = at(L, 4);
            auto v3 = at(L, 5);
            auto v4 = at(L, 6);
            auto v5 = at(L, 7);
            rv = fnd->second(L, { v0.get(), v1.get(), v2.get(), v3.get(), v4.get(), v5.get() });
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
            rv = fnd->second(L, { v0.get(), v1.get(), v2.get(), v3.get(), v4.get(), v5.get(), v6.get() });
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
            rv = fnd->second(L, { v0.get(), v1.get(), v2.get(), v3.get(), v4.get(), v5.get(), v6.get(), v7.get() });
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
            rv = fnd->second(L, { v0.get(), v1.get(), v2.get(), v3.get(), v4.get(), v5.get(), v6.get(), v7.get(), v8.get() });
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
    auto L = ctx.d().L;
    NLUA_AUTOSTACK(L);

    auto pctx = &ctx.d();
    auto id = pctx->refId++;

    // 注册到全局对照表中，用于激活函数时查找真正的执行函数
    pctx->refFuncs[id] = [&](lua_State *L, args_type const &args) -> return_type {
        try {
            NLUA_DEBUG("调用C++函数： " << name);
            return this->func(args);
        }
        catch (error &e) {
            string err = "lua调用C++: " + name + " 遇到异常: " + e.what();
            NLUA_ERROR(err);
            luaL_error(L, err.c_str());
        }
        catch (...) {
            string err = "lua调用C++: " + name + " 遇到未处理异常";
            NLUA_ERROR(err);
            luaL_error(L, err.c_str());
        }
        return nullptr;
    };

    lua_pushlightuserdata(L, pctx);
    lua_pushinteger(L, id);
    lua_pushcclosure(L, private_class_type::ImpStaticFunction, 2);

    lua_setglobal(L, name.c_str());
}

void Function::declare_in(Context &ctx, Class const &clz) const {
    auto L = ctx.d().L;
    NLUA_AUTOSTACK(L);

    auto pctx = &ctx.d();
    auto id = pctx->refId++; // 函数id，之后回调通过id找到具体的实现

    // 绑定到类
    int clzid = lua_gettop(L);
    lua_pushstring(L, name.c_str());
    lua_pushlightuserdata(L, pctx);
    lua_pushinteger(L, id);

    if (classfunc) {
        // 注册到全局对照表中，用于激活函数时查找真正的执行函数
        pctx->refClassFuncs[id] = [&](lua_State *L, self_type &self, args_type const &args) -> return_type {
            try {
                // NLUA_DEBUG("调用C++成员函数： " << name);

                // 如果是单件，则self换为使用全局名称获得
                if (clz.singleton()) {
                    self->d().id = 0;
                    self->d().name = clz.singleton()->_globalvar;
                }
                
                return this->classfunc(self, args);
            }
            catch (error &e) {
                string err = "lua调用C++: " + name + " 遇到异常: " + e.what();
                NLUA_ERROR(err);
                luaL_error(L, err.c_str());
            }
            catch (...) {
                string err = "lua调用C++: " + name + "@" + clz.name + " 遇到未处理异常";
                NLUA_ERROR(err);
                luaL_error(L, err.c_str());
            }
            return nullptr;
        };
        lua_pushcclosure(L, private_class_type::ImpClassFunction, 2);
    }
    else {
        // 注册静态函数
        pctx->refFuncs[id] = [&](lua_State *L, args_type const &args) -> return_type {
            try {
                // NLUA_DEBUG("调用C++静态函数： " << name);

                return this->func(args);
            }
            catch (error &e) {
                string err = "lua调用C++: " + name + " 遇到异常: " + e.what();
                NLUA_ERROR(err);
                luaL_error(L, err.c_str());
            }
            catch (...) {
                string err = "lua调用C++: " + name + "@" + clz.name + " 遇到未处理异常";
                NLUA_ERROR(err);
                luaL_error(L, err.c_str());
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
        auto L = ctx.d().L;
        NLUA_AUTOSTACK(L);

        for (auto &e : supers) {
            if (e.clazz) {
                e.clazz->d_ptr->body_declare_in(ctx, _curclass);
            }
            else if (e.object) {
                Object t; // 获得当前类对象，将父类进行keyvalue设置
                t.d_ptr->id = lua_gettop(L);
                e.object->setfor(t);
            }
        }

        for (auto &e : inits) {
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

        // 实例化的对象id
        lua_newtable(L);
        int const objid = lua_gettop(L);

        // 绑定原型到对象
        lua_pushvalue(L, metaid);
        lua_setmetatable(L, objid);

        // 标准c对象是直接返回lua_newuserdata的结果，但是userdata将无法设置kv，所以采用将userdata作为临时变量绑定到普通table中实现
        if (has_fini) {
            // 如果存在析构函数，则认为业务层实现了C++类型自定义，需要呼起释放流程
            // 申请一小段的内存
            lua_newuserdata(L, sizeof(ObjectPrivate::UserData));
            auto ud = (ObjectPrivate::UserData *) lua_touserdata(L, -1);
            ud->data = nullptr;
            ud->freed = false;

            // 将c类型保存
            lua_pushstring(L, "__cdata__"); // -1
            lua_pushvalue(L, -2); // ud
            lua_rawset(L, objid);
        }

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
                NLUA_DEBUG("调用构造函数失败");
                // 初始化失败，传入空
                lua_pushnil(L);
                return 1;
            }
        }

        // 将实例对象放到栈顶
        lua_pushvalue(L, objid);
        return 1;
    }

    // 析构函数实现
    static int ImpDestroy(lua_State *L) {
        // self 1

        // 如果已经释放，则不调用
        lua_pushstring(L, "__cdata__");
        lua_rawget(L, 1);
        if (!lua_isuserdata(L, -1)) {
            NLUA_ERROR("ImpDestroy被非C++对象调用");
            return 0; // 不存在c数据
        }

        auto ud = (ObjectPrivate::UserData *) lua_touserdata(L, -1);
        if (ud->freed) {
            NLUA_ERROR("ImpDestroy重复释放");
            return 0; // 已经释放
        }
        // 弹出ud
        lua_pop(L, 1);

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

            // 调用原始的new函数实例化
            {
                // 1 clzid
                lua_pushcfunction(L, ContextPrivate::Traceback);

                // 2 tbid
                lua_pushstring(L, "__singleton_new__");
                lua_rawget(L, 1);
                lua_pushvalue(L, 1);

                int s = lua_pcall(L, 1, 1, 2);
                if (s != LUA_OK) {
                    NLUA_DEBUG("调用单件的构造函数失败");
                    lua_pushnil(L);
                }
            }

            // 单件对象id
            int objid = lua_gettop(L);

            // 设置到__shared上
            lua_pushstring(L, "__shared");
            lua_pushvalue(L, objid);
            lua_rawset(L, 1);

            // 获得对应的C++定义
            auto sig = (Singleton *)lua_topointer(L, lua_upvalueindex(1));
            auto pctx = (ContextPrivate *)lua_topointer(L, lua_upvalueindex(2));

            // 绑定到全局中
            ostringstream oss;
            oss << "__global_singleton_" << pctx->refSingletonId++;
            sig->_globalvar = oss.str();
            lua_pushvalue(L, objid);
            lua_setglobal(L, sig->_globalvar.c_str());

            // 返回对象
            lua_pushvalue(L, objid);

            // 获得singleton对象，用于调用初始化函数
            if (sig->init) {
                auto self = make_shared<Object>();
                self->d_ptr->name = sig->_globalvar;
                self->d_ptr->L = L;
                sig->init(self);
            }
        }
        return 1;
    }

    static int ImpFreeSingleton(lua_State *L) {
        lua_pushstring(L, "__shared");
        lua_rawget(L, 1);
        if (!lua_isnil(L, -1)) {
            int objid = lua_gettop(L);

            auto sig = (Singleton *)lua_topointer(L, lua_upvalueindex(1));

            if (sig->fini) {
                auto self = make_shared<Object>();
                self->d_ptr->L = L;
                self->d_ptr->id = objid;
                sig->fini(self);
            }

            // 释放
            lua_pushstring(L, "__shared");
            lua_pushnil(L);
            lua_rawset(L, 1);

            // 释放对应的全局变量
            lua_pushnil(L);
            lua_setglobal(L, sig->_globalvar.c_str());
        }
        return 1;
    }

    shared_ptr<Singleton> singleton;
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

Class &Class::singleton(string const &_name, Singleton::ini_type _init, Singleton::ini_type _fini) {
    auto t = make_shared<Singleton>();
    t->name = _name;
    t->init = move(_init);
    t->fini = move(_fini);
    d_ptr->singleton = t;
    return *this;
}

Class::singleton_type Class::singleton() const {
    return d_ptr->singleton;
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

#define IMPFUNC_CHECK_ARGS(count)        \
    do                                   \
    {                                    \
        if (args.size() < count)         \
            throw error(-1, "缺少参数"); \
    } while (0)

Class &Class::init(Function::basefunc1_type func) {
    return init([=](self_type &self, args_type const &args) {
        IMPFUNC_CHECK_ARGS(1);
        return func(self, COMXX_PPARGS_1(**, args));
        }, 1);
}

Class &Class::init(Function::basefunc2_type func) {
    return init([=](self_type &self, args_type const &args) {
        IMPFUNC_CHECK_ARGS(2);
        return func(self, COMXX_PPARGS_2(**, args));
        }, 2);
}

Class &Class::init(Function::basefunc3_type func) {
    return init([=](self_type &self, args_type const &args) {
        IMPFUNC_CHECK_ARGS(3);
        return func(self, COMXX_PPARGS_3(**, args));
        }, 3);
}

Class &Class::init(Function::basefunc4_type func) {
    return init([=](self_type &self, args_type const &args) {
        IMPFUNC_CHECK_ARGS(4);
        return func(self, COMXX_PPARGS_4(**, args));
        }, 4);
}

Class &Class::init(Function::basefunc5_type func) {
    return init([=](self_type &self, args_type const &args) {
        IMPFUNC_CHECK_ARGS(5);
        return func(self, COMXX_PPARGS_5(**, args));
        }, 5);
}

Class &Class::init(Function::basefunc6_type func) {
    return init([=](self_type &self, args_type const &args) {
        IMPFUNC_CHECK_ARGS(6);
        return func(self, COMXX_PPARGS_6(**, args));
        }, 6);
}

Class &Class::init(Function::basefunc7_type func) {
    return init([=](self_type &self, args_type const &args) {
        IMPFUNC_CHECK_ARGS(7);
        return func(self, COMXX_PPARGS_7(**, args));
        }, 7);
}

Class &Class::init(Function::basefunc8_type func) {
    return init([=](self_type &self, args_type const &args) {
        IMPFUNC_CHECK_ARGS(8);
        return func(self, COMXX_PPARGS_8(**, args));
        }, 8);
}

Class &Class::init(Function::basefunc9_type func) {
    return init([=](self_type &self, args_type const &args) {
        IMPFUNC_CHECK_ARGS(9);
        return func(self, COMXX_PPARGS_9(**, args));
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
        return func(self, COMXX_PPARGS_1(**, args));
        });
}

Class &Class::add(string const &fname, Function::classfunc2_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(2);
        return func(self, COMXX_PPARGS_2(**, args));
        });
}

Class &Class::add(string const &fname, Function::classfunc3_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(3);
        return func(self, COMXX_PPARGS_3(**, args));
        });
}

Class &Class::add(string const &fname, Function::classfunc4_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(4);
        return func(self, COMXX_PPARGS_4(**, args));
        });
}

Class &Class::add(string const &fname, Function::classfunc5_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(5);
        return func(self, COMXX_PPARGS_5(**, args));
        });
}

Class &Class::add(string const &fname, Function::classfunc6_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(6);
        return func(self, COMXX_PPARGS_6(**, args));
        });
}

Class &Class::add(string const &fname, Function::classfunc7_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(7);
        return func(self, COMXX_PPARGS_7(**, args));
        });
}

Class &Class::add(string const &fname, Function::classfunc8_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(8);
        return func(self, COMXX_PPARGS_8(**, args));
        });
}

Class &Class::add(string const &fname, Function::classfunc9_type func) {
    return add(fname, [=](self_type &self, args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(9);
        return func(self, COMXX_PPARGS_9(**, args));
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
        return func(COMXX_PPARGS_1(**, args));
        });
}

Class &Class::add_static(string const &fname, Function::func2_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(2);
        return func(COMXX_PPARGS_2(**, args));
        });
}

Class &Class::add_static(string const &fname, Function::func3_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(3);
        return func(COMXX_PPARGS_3(**, args));
        });
}

Class &Class::add_static(string const &fname, Function::func4_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(4);
        return func(COMXX_PPARGS_4(**, args));
        });
}

Class &Class::add_static(string const &fname, Function::func5_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(5);
        return func(COMXX_PPARGS_5(**, args));
        });
}

Class &Class::add_static(string const &fname, Function::func6_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(6);
        return func(COMXX_PPARGS_6(**, args));
        });
}

Class &Class::add_static(string const &fname, Function::func7_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(7);
        return func(COMXX_PPARGS_7(**, args));
        });
}

Class &Class::add_static(string const &fname, Function::func8_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(8);
        return func(COMXX_PPARGS_8(**, args));
        });
}

Class &Class::add_static(string const &fname, Function::func9_type func) {
    return add_static(fname, [=](args_type const &args) -> return_type {
        IMPFUNC_CHECK_ARGS(9);
        return func(COMXX_PPARGS_9(**, args));
        });
}

Class &Class::add(field_type const &f) {
    d_ptr->fields[f->name] = f;
    return *this;
}

void Singleton::declare_in(Context &ctx) const {
    auto pctx = &ctx.d();
    auto L = pctx->L;
    NLUA_AUTOSTACK(L);

    int clzid = lua_gettop(L);

    lua_pushstring(L, name.c_str());
    lua_pushlightuserdata(L, (void *)this);
    lua_pushlightuserdata(L, (void *)pctx);
    lua_pushcclosure(L, ClassPrivate::ImpGetSingleton, 2);
    lua_rawset(L, clzid);

    lua_pushstring(L, ("free_" + name).c_str());
    lua_pushlightuserdata(L, (void *)this);
    lua_pushlightuserdata(L, (void *)pctx);
    lua_pushcclosure(L, ClassPrivate::ImpFreeSingleton, 2);
    lua_rawset(L, clzid);

    lua_pushstring(L, "__shared");
    lua_pushnil(L);
    lua_rawset(L, clzid);

    lua_pushstring(L, "__singleton_new__");
    lua_pushcfunction(L, ClassPrivate::ImpNew);
    lua_rawset(L, clzid);
}

void Class::declare_in(Context &ctx) const {
    auto L = ctx.d().L;
    NLUA_AUTOSTACK(L);

    lua_newtable(L);
    int clzid = lua_gettop(L);

    if (d_ptr->singleton) {
        d_ptr->singleton->declare_in(ctx);
    }
    else {
        lua_pushstring(L, "new");
        lua_pushcfunction(L, ClassPrivate::ImpNew);
        lua_rawset(L, clzid);
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

// 获得module的定义栈id
extern int DeclareModule(Module const&, Context &);

void Class::declare_in(Context &ctx, Module const &mod) const {
    auto L = ctx.d().L;
    NLUA_AUTOSTACK(L);

    lua_newtable(L);
    int clzid = lua_gettop(L);

    if (d_ptr->singleton) {
        d_ptr->singleton->declare_in(ctx);
    }
    else {
        lua_pushstring(L, "new");
        lua_pushcfunction(L, ClassPrivate::ImpNew);
        lua_rawset(L, clzid);
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
    int modid = DeclareModule(mod, ctx);
    lua_pushstring(L, name.c_str());
    lua_pushvalue(L, clzid);
    lua_rawset(L, modid);
}

class ModulePrivate {
public:
    Module *d_owner;

    Module::classes_type classes;
    Module::modules_type modules;

    int declare_module(Context &ctx) const {
        auto L = ctx.d().L;
        auto pars = d_owner->parents();
        pars.emplace_back(d_owner);
        int modid = 0;

        auto first = pars[0];
        lua_getglobal(L, first->name.c_str());
        if (!lua_isnil(L, -1)) {
            modid = lua_gettop(L);
        }
        else {
            lua_newtable(L);
            modid = lua_gettop(L);
            lua_pushstring(L, "__index");
            lua_pushvalue(L, modid);
            lua_rawset(L, modid);
            lua_setglobal(L, first->name.c_str());
        }

        for (size_t i = 1; i < pars.size(); ++i) {
            auto m = pars[i];
            lua_pushstring(L, m->name.c_str());
            lua_rawget(L, modid);
            if (!lua_isnil(L, -1)) {
                modid = lua_gettop(L);
            }
            else {
                lua_newtable(L);
                int submodid = lua_gettop(L);
                lua_pushstring(L, "__index");
                lua_pushvalue(L, submodid);
                lua_rawset(L, submodid);

                lua_pushstring(L, m->name.c_str());
                lua_pushvalue(L, submodid);
                lua_rawset(L, modid);

                modid = submodid;
            }
        }

        return modid;
    }

    void body_declare_in(Context &ctx) {
        for (auto &e : classes) {
            e.second->declare_in(ctx, *d_owner);
        }

        for (auto &e : modules) {
            e.second->declare_in(ctx);
        }
    }
};

int DeclareModule(Module const& m, Context& ctx) {
    return m.d().declare_module(ctx);
}

Module::Module() {
    NNT_CLASS_CONSTRUCT();
    d_ptr->d_owner = this;
}

Module::~Module() {
    d_ptr->d_owner = nullptr;
    NNT_CLASS_DESTORY()
}

bool Module::add(class_type const& c) {
    auto fnd = d_ptr->classes.find(c->name);
    if (fnd != d_ptr->classes.end()) {
        NLUA_ERROR("模块中已经存在类 " << c->name);
        return false;
    }
    d_ptr->classes[c->name] = c;
    return true;
}

bool Module::add(module_type const& m) {
    if (m->parent) {
        NLUA_ERROR(name << " 已经是 " << m->parent->name << " 的子模块");
        return false;
    }
    auto fnd = d_ptr->modules.find(m->name);
    if (fnd != d_ptr->modules.end()) {
        fnd->second->merge(*m);
    }
    else {
        d_ptr->modules[m->name] = m;
    }
    m->parent = this;
    return true;
}

void Module::declare_in(Context &ctx) const {
    auto L = ctx.d().L;
    NLUA_AUTOSTACK(L);

    d_ptr->declare_module(ctx);
    d_ptr->body_declare_in(ctx);
}

void Module::merge(Module const &r) {
    for (auto &e : r.d_ptr->classes) {
        d_ptr->classes[e.first] = e.second;
    }

    for (auto &e : r.d_ptr->modules) {
        auto fnd = d_ptr->modules.find(e.first);
        if (fnd != d_ptr->modules.end()) {
            fnd->second->merge(*e.second);
        }
        else {
            d_ptr->modules[e.first] = e.second;
            e.second->parent = this;
        }
    }
}

Module::parents_type Module::parents() const {
    parents_type r;
    auto p = parent;
    while (p) {
        r.emplace_back(p);
        p = p->parent;
    }
    return parents_type(r.rbegin(), r.rend());
}

Object::Object() {
    NNT_CLASS_CONSTRUCT()
}

Object::~Object() {
    NNT_CLASS_DESTORY()
}

void *Object::payload() const {
    auto L = d_ptr->GL ? d_ptr->GL : d_ptr->L;
    NLUA_AUTOSTACK(L);

    if (d_ptr->id) {
        lua_pushvalue(L, d_ptr->id);
    }
    else {
        lua_getglobal(L, d_ptr->name.c_str());
    }

    int objid = lua_gettop(L);
    lua_pushstring(L, "__cdata__");
    lua_rawget(L, objid);

    if (!lua_isuserdata(L, -1)) {
        NLUA_ERROR("不是C++自定义对象");
        return nullptr;
    }

    auto ud = (ObjectPrivate::UserData *) lua_touserdata(L, -1);
    return ud->data;
}

void Object::payload(void *data) {
    auto L = d_ptr->GL ? d_ptr->GL : d_ptr->L;
    NLUA_AUTOSTACK(L);

    if (d_ptr->id) {
        lua_pushvalue(L, d_ptr->id);
    }
    else {
        lua_getglobal(L, d_ptr->name.c_str());
    }

    int objid = lua_gettop(L);
    lua_pushstring(L, "__cdata__");
    lua_rawget(L, objid);

    if (!lua_isuserdata(L, -1)) {
        NLUA_ERROR("不是C++自定义对象");
        return;
    }

    auto ud = (ObjectPrivate::UserData *) lua_touserdata(L, -1);
    ud->data = data;
}

return_type Object::get(string const &name) const {
    auto L = d_ptr->GL ? d_ptr->GL : d_ptr->L;
    NLUA_AUTOSTACK(L);

    if (d_ptr->id) {
        lua_pushvalue(L, d_ptr->id);
    }
    else {
        lua_getglobal(L, d_ptr->name.c_str());
    }

    lua_pushstring(L, name.c_str());
    lua_rawget(L, -2);

    return at(L, -1);
}

void Object::set(string const &name, value_type const &val) {
    auto L = d_ptr->GL ? d_ptr->GL : d_ptr->L;
    NLUA_AUTOSTACK(L);

    if (d_ptr->id) {
        // 局部变量
        lua_pushvalue(L, d_ptr->id);
    }
    else {
        // 全局变量
        lua_getglobal(L, d_ptr->name.c_str());
    } // -3

    lua_pushstring(L, name.c_str()); // -2
    push(val.get(), L); // -1

    if (!lua_istable(L, -3)) {
        NLUA_ERROR("Object::set 可能遇到并发写冲突导致原始对象丢失");
        return;
    }

    // obj[name] = val
    lua_rawset(L, -3);
}

self_type Context::global(string const &name) const {
    auto L = d_ptr->L;

    if (name.find('.') != string::npos) {
        // 多级模块
        auto ss = explode(name, ".");
        lua_getglobal(L, ss[0].c_str());
        if (!lua_istable(L, -1))
            return nullptr;
        int cid = lua_gettop(L);
        for (auto iter = ss.begin() + 1; iter != ss.end(); ++iter) {
            lua_pushstring(L, iter->c_str());
            lua_rawget(L, cid);
            if (!lua_istable(L, -1))
                return nullptr;
            cid = lua_gettop(L);
        }

        self_type r = make_shared<Object>();
        r->d_ptr->L = L;
        r->d_ptr->id = cid;
        return r;
    }

    // 根全局对象
    NLUA_AUTOSTACK(L);
    lua_getglobal(L, name.c_str());
    if (lua_isnil(L, -1))
        return nullptr;
    self_type r = make_shared<Object>();
    r->d_ptr->name = name;
    r->d_ptr->L = L;
    return r;
}

self_type Context::singleton(string const& name) const {
    auto obj = global(name);
    if (!obj)
        return nullptr;
    // 获得obj身上的 __shared
    return obj->get("__shared")->toObject();
}

void Object::setfor(Object &r) const {
    auto L = d_ptr->L;
    NLUA_AUTOSTACK(L);

    if (r.d_ptr->id) {
        lua_pushvalue(L, r.d_ptr->id);
    }
    else {
        lua_getglobal(L, r.d_ptr->name.c_str());
    }
    int toid = lua_gettop(L);

    if (d_ptr->id) {
        lua_pushvalue(L, d_ptr->id);
    }
    else {
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
    auto L = d_ptr->GL ? d_ptr->GL : d_ptr->L;
    NLUA_AUTOSTACK(L);

    if (d_ptr->id) {
        // 是局部变量
        lua_getfield(L, d_ptr->id, name.c_str());
        if (lua_isnil(L, -1))
            return false;
    }
    else {
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

bool Object::isnil() const {
    return !d_ptr->L || !d_ptr->id || d_ptr->name.empty();
}

self_type Object::instance() const {
    auto L = d_ptr->GL ? d_ptr->GL : d_ptr->L;
    // NLUA_AUTOSTACK(L); 因需要返回局部变量所以不能恢复

    int clsid;

    if (!d_ptr->name.empty()) {
        lua_getglobal(L, d_ptr->name.c_str());
        if (!lua_istable(L, -1)) {
            NLUA_ERROR("传入的不是存在的类名");
            return nullptr;
        }
        clsid = lua_gettop(L);
    }
    else if (d_ptr->id) {
        if (!lua_istable(L, d_ptr->id)) {
            NLUA_ERROR("传入的不是类对象");
            return nullptr;
        }
        clsid = d_ptr->id;
    }
    else {
        NLUA_ERROR("传入的是空对象");
        return nullptr;
    }

    // meta
    lua_newtable(L);
    int metaid = lua_gettop(L);

    lua_pushstring(L, "__index");
    lua_pushvalue(L, clsid);
    lua_rawset(L, metaid);

    // new
    lua_newtable(L);
    int objid = lua_gettop(L);

    lua_pushvalue(L, metaid);
    lua_setmetatable(L, -2);

    auto r = make_shared<Object>();
    r->d_ptr->L = L;
    r->d_ptr->id = objid;
    return r;
}

void Object::grab() {
    if (!d_ptr->GL) {
        // grab后的操作均位于独立L上进行，此时可以使用Resrouce的锁避免冲突
        d_ptr->GL = GL();

        // 将对象从L搬到GL中，之后都针对GL操作
        if (d_ptr->id) {
            lua_pushvalue(d_ptr->L, d_ptr->id);
            lua_xmove(d_ptr->L, d_ptr->GL, 1);
            d_ptr->id = lua_gettop(d_ptr->GL);
        }
    }

    // auto L = d_ptr->L;
    auto L = d_ptr->GL;
    NLUA_AUTOSTACK(L);

    if (d_ptr->id) {
        // 设置生命期到全局，避免被局部释放
        ostringstream oss;
        oss << "__global_object_" << ObjectPrivate::RefId++;
        d_ptr->name = oss.str();

        lua_pushvalue(L, d_ptr->id);
        lua_setglobal(L, d_ptr->name.c_str());
        d_ptr->id = 0;
    }

    // 已经是全局变量
    lua_getglobal(L, d_ptr->name.c_str());
    if (!lua_istable(L, -1)) {
        // 非table的对象，不维持计数模式
        return;
    }

    int selfid = lua_gettop(L);
    lua_getfield(L, selfid, "__refs");
    if (lua_isnumber(L, -1)) {
        lua_pushinteger(L, lua_tointeger(L, -1) + 1);
    }
    else {
        lua_pushinteger(L, 1);
    }
    lua_setfield(L, selfid, "__refs");
}

bool Object::drop() {
    if (d_ptr->id) {
        NLUA_ERROR("该变量是局部变量，不支持drop");
        return false;
    }

    auto L = d_ptr->GL;
    NLUA_AUTOSTACK(L);

    lua_getglobal(L, d_ptr->name.c_str());
    if (!lua_istable(L, -1)) {
        // 非table的对象，不维持计数模式
        // 需要释放
        lua_pushnil(L);
        lua_setglobal(L, d_ptr->name.c_str());
        return true;
    }

    int selfid = lua_gettop(L);
    lua_getfield(L, selfid, "__refs");
    if (!lua_isnumber(L, -1)) {
        NLUA_ERROR("该变量没有进行过grab操作");
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
    auto L = d_ptr->GL ? d_ptr->GL : d_ptr->L;
    NLUA_AUTOSTACK(L);

    lua_pushcfunction(L, ContextPrivate::Traceback);
    int tbid = lua_gettop(L);

    if (d_ptr->id) {
        // 是局部变量
        if (!lua_istable(L, d_ptr->id)) {
            NLUA_DEBUG("不能调用非table对象身上的 " << name << " 函数");
            return nullptr;
        }
        lua_getfield(L, d_ptr->id, name.c_str());
        if (!lua_isfunction(L, -1)) {
            NLUA_DEBUG("没有找到函数 " << name);
            return nullptr;
        }

        // push self
        lua_pushvalue(L, d_ptr->id);
    }
    else {
        // 获得全局变量
        lua_getglobal(L, d_ptr->name.c_str());
        if (!lua_istable(L, -1)) {
            NLUA_DEBUG("没有找到全局变量 " << d_ptr->name);
            return nullptr;
        }
        int selfid = lua_gettop(L);

        // 获得的函数
        lua_getfield(L, -1, name.c_str());
        if (!lua_isfunction(L, -1)) {
            NLUA_DEBUG("没有找到成员函数 " << name << "@" << d_ptr->name);
            return nullptr;
        }

        // push self
        lua_pushvalue(L, selfid);
    }

    for (auto &e : args) {
        push(e, L);
    }

    int s = lua_pcall(L, args.size() + 1, 1, tbid);
    if (LUA_OK != s) {
        NLUA_DEBUG("函数调用失败");
        return nullptr;
    }
    return at(L, -1);
}

return_type Object::invoke(string const &name) {
    return invoke(name, {});
}

return_type Object::invoke(string const &name, Variant const &v0) {
    return invoke(name, { &v0 });
}

return_type Object::invoke(string const &name, Variant const &v0, Variant const &v1) {
    return invoke(name, { &v0, &v1 });
}

return_type Object::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2) {
    return invoke(name, { &v0, &v1, &v2 });
}

return_type Object::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2, Variant const &v3) {
    return invoke(name, { &v0, &v1, &v2, &v3 });
}

return_type Object::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2, Variant const &v3, Variant const &v4) {
    return invoke(name, { &v0, &v1, &v2, &v3, &v4 });
}

return_type Object::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2, Variant const &v3, Variant const &v4, Variant const &v5) {
    return invoke(name, { &v0, &v1, &v2, &v3, &v4, &v5 });
}

return_type Object::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2, Variant const &v3, Variant const &v4, Variant const &v5, Variant const &v6) {
    return invoke(name, { &v0, &v1, &v2, &v3, &v4, &v5, &v6 });
}

return_type Object::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2, Variant const &v3, Variant const &v4, Variant const &v5, Variant const &v6, Variant const &v7) {
    return invoke(name, { &v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7 });
}

return_type Object::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2, Variant const &v3, Variant const &v4, Variant const &v5, Variant const &v6, Variant const &v7, Variant const &v8) {
    return invoke(name, { &v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8 });
}

return_type Object::invoke(string const &name, Variant const &v0, Variant const &v1, Variant const &v2, Variant const &v3, Variant const &v4, Variant const &v5, Variant const &v6, Variant const &v7, Variant const &v8, Variant const &v9) {
    return invoke(name, { &v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, &v9 });
}

return_type Object::call(args_type const& args) {
    auto L = d_ptr->GL ? d_ptr->GL : d_ptr->L;
    NLUA_AUTOSTACK(L);

    lua_pushcfunction(L, ContextPrivate::Traceback);
    int tbid = lua_gettop(L);

    if (d_ptr->id) {
        // 是局部函数
        if (!lua_isfunction(L, d_ptr->id)) {
            NLUA_DEBUG("Object::call 对象不是函数");
            return nullptr;
        }

        lua_pushvalue(L, d_ptr->id);
    }
    else {
        // 获得全局函数
        lua_getglobal(L, d_ptr->name.c_str());
        if (!lua_isfunction(L, -1)) {
            NLUA_DEBUG("Object::call 没有找到全局函数 " << d_ptr->name);
            return nullptr;
        }
    }

    for (auto& e : args) {
        push(e, L);
    }

    int s = lua_pcall(L, args.size(), 1, tbid);
    if (LUA_OK != s) {
        NLUA_DEBUG("函数执行失败");
        return nullptr;
    }
    return at(L, -1);
}

return_type Object::call() {
    return call({});
}

return_type Object::call(Variant const& v0) {
    return call({ &v0 });
}

return_type Object::call(Variant const& v0, Variant const& v1) {
    return call({ &v0, &v1 });
}

return_type Object::call(Variant const& v0, Variant const& v1, Variant const& v2) {
    return call({ &v0, &v1, &v2 });
}

return_type Object::call(Variant const& v0, Variant const& v1, Variant const& v2, Variant const& v3) {
    return call({ &v0, &v1, &v2, &v3 });
}

return_type Object::call(Variant const& v0, Variant const& v1, Variant const& v2, Variant const& v3, Variant const& v4) {
    return call({ &v0, &v1, &v2, &v3, &v4 });
}

return_type Object::call(Variant const& v0, Variant const& v1, Variant const& v2, Variant const& v3, Variant const& v4, Variant const& v5) {
    return call({ &v0, &v1, &v2, &v3, &v4, &v5 });
}

return_type Object::call(Variant const& v0, Variant const& v1, Variant const& v2, Variant const& v3, Variant const& v4, Variant const& v5, Variant const& v6) {
    return call({ &v0, &v1, &v2, &v3, &v4, &v5, &v6 });
}

return_type Object::call(Variant const& v0, Variant const& v1, Variant const& v2, Variant const& v3, Variant const& v4, Variant const& v5, Variant const& v6, Variant const& v7) {
    return call({ &v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7 });
}

return_type Object::call(Variant const& v0, Variant const& v1, Variant const& v2, Variant const& v3, Variant const& v4, Variant const& v5, Variant const& v6, Variant const& v7, Variant const& v8) {
    return call({ &v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8 });
}

return_type Object::call(Variant const& v0, Variant const& v1, Variant const& v2, Variant const& v3, Variant const& v4, Variant const& v5, Variant const& v6, Variant const& v7, Variant const& v8, Variant const& v9) {
    return call({ &v0, &v1, &v2, &v3, &v4, &v5, &v6, &v7, &v8, &v9 });
}

shared_ptr<Variant> Object::toVariant() const
{
    integer oid = d_ptr->id;
    if (!oid) {
        if (d_ptr->name.empty()) {
            NLUA_ERROR("该变量既不是局部变量也不是全局变量不能转换成Variant");
            return nullptr;
        }

        auto L = d_ptr->GL ? d_ptr->GL : d_ptr->L;
        lua_getglobal(L, d_ptr->name.c_str());
        oid = lua_gettop(L);

        // 如果位于GL上，需要放回L中
        if (d_ptr->GL) {
            lua_xmove(d_ptr->GL, d_ptr->L, 1);
            oid = lua_gettop(d_ptr->L);
        }
    }

    auto r = make_shared<Variant>(oid);
    const_cast<Variant::VT&>(r->vt) = Variant::VT::OBJECT;
    return r;
}

shared_ptr<Variant::com_variant_type> Object::toComVariant() const
{
    if (d_ptr->id)
        return make_shared<Variant::com_variant_type>(d_ptr->id);
    return make_shared<Variant::com_variant_type>(d_ptr->name);
}

self_type to_object(lua_State* L, int n)
{
    auto r = make_shared<Object>();
    auto& rd = r->d();
    rd.L = L;
    rd.id = n;
    return r;
}

NLUA_END
