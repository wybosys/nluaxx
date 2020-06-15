#ifndef __NLUA_AST_PRIVATE_H_INCLUDED
#define __NLUA_AST_PRIVATE_H_INCLUDED

#include "liblua.hpp"
#include <atomic>

NLUA_BEGIN

typedef ::std::atomic<lua_Integer> lua_refid_type;
typedef function<return_type(lua_State *L, self_type &self, args_type const &)> luaref_classfunc_type;
typedef function<return_type(lua_State *L, args_type const &)> luaref_func_type;
typedef ::std::map<lua_Integer, luaref_classfunc_type> luaref_classfuncs_type;
typedef ::std::map<lua_Integer, luaref_func_type> luaref_funcs_type;

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

    static int Traceback(lua_State *L);

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
};

NLUA_END

#endif