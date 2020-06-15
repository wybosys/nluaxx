#ifndef __NLUA_XX_H_INCLUDED
#define __NLUA_XX_H_INCLUDED

#include "com++.hpp"
#include "macro.hpp"

#include <map>
#include <string>
#include <memory>
#include <iostream>
#include <cstring>
#include <functional>
#include <initializer_list>

#ifndef NLUA_NS
#define NLUA_NS nlua
#endif

#define NLUA_BEGIN    \
    namespace NLUA_NS \
    {
#define NLUA_END }
#define USE_NLUA using namespace NLUA_NS;

#define NLUA_STATIC_INVOKE(func) \
[&](args_type const& args)->return_type { \
return make_shared<Variant>(com::function_call<decltype(&func)>()(&func, args)); \
}

NLUA_BEGIN

using ::std::string;
using ::std::shared_ptr;
using ::std::make_shared;
using ::std::function;

class error : public ::std::exception
{
public:
    explicit error(int code, string const &msg = "") : _code(code), _msg(msg) {}

    virtual const char *what() const throw()
    {
        return _msg.c_str();
    }

private:
    int _code;
    string _msg;
};

class Class;

class Object;

class Field;

class Function;

class Module;

class Variant;

typedef shared_ptr<Variant> return_type;
typedef shared_ptr<Variant> value_type;
typedef Variant arg_type;
typedef ::std::initializer_list<arg_type const*> args_type;
typedef shared_ptr<Object> self_type;

class Any {
public:
    Any(shared_ptr<Class> const &r) : clazz(r) {}

    Any(shared_ptr<Object> const &r) : object(r) {}

    shared_ptr<Class> clazz;
    shared_ptr<Object> object;
};

NNT_CLASS_PREPARE(Context);

class Context {
    NNT_CLASS_DECL(Context);

public:

    NNT_SINGLETON_DECL(Context);

    // 创建一个空上下文
    Context();

    ~Context();

    // 绑定已经存在的lua_State上下文
    Context &attach(void *);

    // 创建新的lua_State上下文
    Context &create();

    // 加载文件
    bool load(string const &);

    // 执行函数
    return_type invoke(string const &fname, args_type const &args = {});

    return_type invoke(string const &name, Variant const &);

    return_type invoke(string const &name, Variant const &, Variant const &);

    return_type invoke(string const &name, Variant const &, Variant const &, Variant const &);

    return_type invoke(string const &name, Variant const &, Variant const &, Variant const &, Variant const &);

    return_type invoke(string const &name, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &);

    return_type invoke(string const &name, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &);

    return_type invoke(string const &name, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &);

    return_type invoke(string const &name, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &);

    return_type invoke(string const &name, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &);

    return_type invoke(string const &name, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &);

    // 添加查找包的目录
    void add_package_path(string const &);

    void add_cpackage_path(string const &);

    // 获得全局对象
    self_type global(string const &);

    typedef shared_ptr<Module> module_type;
    typedef ::std::map<string, module_type> modules_type;

    typedef shared_ptr<Class> class_type;
    typedef ::std::map<string, class_type> classes_type;

    // 全局锁，用来避免多线程环境中调用lua出错
    void lock();
    void unlock();

    // 清空
    void clear();

    // 增加类定义
    void add(class_type &);

    // 增加模块定义
    void add(module_type &);

    // 声明
    void declare();
};

NLUA_END

#include "ast.hpp"

#endif
