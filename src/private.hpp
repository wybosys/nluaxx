#ifndef __NLUA_AST_PRIVATE_H_INCLUDED
#define __NLUA_AST_PRIVATE_H_INCLUDED

#ifndef __NLUA_PRIVATE__
#error "禁止在外部引用该文件"
#endif

#include "liblua.hpp"
#include <atomic>
#include <mutex>

#include <cross/cross.hpp>
#include <cross/sys.hpp>

NLUA_BEGIN

typedef ::std::atomic<lua_Integer> lua_refid_type;
typedef function<return_type(lua_State *L, self_type &self, args_type const &)> luaref_classfunc_type;
typedef function<return_type(lua_State *L, args_type const &)> luaref_func_type;
typedef ::std::map<lua_Integer, luaref_classfunc_type> luaref_classfuncs_type;
typedef ::std::map<lua_Integer, luaref_func_type> luaref_funcs_type;

class ContextAutoGuard
{
public:

    ContextAutoGuard();
    ~ContextAutoGuard();

    lua_State *L = nullptr; // 当前线程的环境
    bool ismain = false; // 是否时主线程
    ::CROSS_NS::tid_t tid; // 当前的线程号
    
    static lua_State *MainL; // 主线程L
    static thread_local ContextAutoGuard Tls; // 当前线程
};

class ContextPrivate
{
public:

    ContextPrivate();
    ~ContextPrivate();

    void clear();
    void create();
    void attach(lua_State*);
    string find_file(string const &file);
    void update_package_paths(::std::vector<string> const *curs);
    void update_cpackage_paths(::std::vector<string> const *curs);

    // 通用lua层错误处理函数
    static int Traceback(lua_State *L);

    // lua原始句柄
    lua_State *L = nullptr;
    bool _freel = false;

    ::std::vector<string> package_paths, cpackage_paths;

    // 当前声明中的类和模块
    Context::classes_type classes;
    Context::modules_type modules;

    // 保存lua实现的成员函数、类函数和C++实现之间的关系
    lua_refid_type refId;
    luaref_funcs_type refFuncs;
    luaref_classfuncs_type refClassFuncs;

    // 全局singleton计数器
    lua_refid_type refSingletonId;

    // 各种锁
    ::std::mutex mtx_global;
};

NLUA_END

#endif