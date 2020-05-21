#ifndef __NLUA_XX_H_INCLUDED
#define __NLUA_XX_H_INCLUDED

#include "core.h"
#include "variant.h"
#include "fs.h"
#include <initializer_list>
#include <map>

NLUA_BEGIN

class Class;

class Object;

class Field;

class Function;

class Module;

typedef shared_ptr<Variant> return_type;
typedef shared_ptr<Variant> value_type;
typedef initializer_list<Variant> args_type;
typedef shared_ptr<Object> self_type;

class Any {
public:
    Any(shared_ptr<Class> const &r) : clazz(r) {}

    Any(shared_ptr<Object> const &r) : object(r) {}

    shared_ptr<Class> clazz;
    shared_ptr<Object> object;
};

NLUA_CLASS_PREPARE(Context);

class Context {
NLUA_CLASS_DECL(Context);

public:
    // 初始化一个全局的 *为了业务
    static Context shared;

    // 初始化
    Context();

    // 连接一个已经存在的
    explicit Context(void *);

    // 以null初始化后可以连接一个已经存在的
    void attach(void *);

    ~Context();

    // 加载文件
    bool load(path const &);

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
    void add_package_path(path const &);

    void add_cpackage_path(path const &);

    // 获得全局对象
    self_type global(string const &);

    typedef shared_ptr<Module> module_type;
    typedef ::std::map<string, module_type> modules_type;

    typedef shared_ptr<Class> class_type;
    typedef ::std::map<string, class_type> classes_type;

    // 清空
    void clear();

    // 增加类定义
    void add(class_type &);

    // 增加模块定义
    void add(module_type &);

    // 声明
    void declare();
};

NLUA_CLASS_PREPARE(Function)

class Function {
NLUA_CLASS_DECL(Function)

public:
    Function();

    ~Function();

    string name;

    // 构造析构函数
    typedef function<void(self_type &self, args_type const &)> basefunc_type;

    typedef function<void(self_type &self)> basefunc0_type;
    typedef function<void(self_type &self, Variant const &)> basefunc1_type;
    typedef function<void(self_type &self, Variant const &, Variant const &)> basefunc2_type;
    typedef function<void(self_type &self, Variant const &, Variant const &, Variant const &)> basefunc3_type;
    typedef function<void(self_type &self, Variant const &, Variant const &, Variant const &, Variant const &)> basefunc4_type;
    typedef function<void(self_type &self, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> basefunc5_type;
    typedef function<void(self_type &self, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> basefunc6_type;
    typedef function<void(self_type &self, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> basefunc7_type;
    typedef function<void(self_type &self, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> basefunc8_type;
    typedef function<void(self_type &self, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> basefunc9_type;

    // 类函数
    typedef function<return_type(self_type &self, args_type const &)> classfunc_type;

    typedef function<return_type(self_type &self)> classfunc0_type;
    typedef function<return_type(self_type &self, Variant const &)> classfunc1_type;
    typedef function<return_type(self_type &self, Variant const &, Variant const &)> classfunc2_type;
    typedef function<return_type(self_type &self, Variant const &, Variant const &, Variant const &)> classfunc3_type;
    typedef function<return_type(self_type &self, Variant const &, Variant const &, Variant const &, Variant const &)> classfunc4_type;
    typedef function<return_type(self_type &self, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> classfunc5_type;
    typedef function<return_type(self_type &self, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> classfunc6_type;
    typedef function<return_type(self_type &self, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> classfunc7_type;
    typedef function<return_type(self_type &self, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> classfunc8_type;
    typedef function<return_type(self_type &self, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> classfunc9_type;

    // 静态函数
    typedef function<return_type(args_type const &)> func_type;

    typedef function<return_type()> func0_type;
    typedef function<return_type(Variant const &)> func1_type;
    typedef function<return_type(Variant const &, Variant const &)> func2_type;
    typedef function<return_type(Variant const &, Variant const &, Variant const &)> func3_type;
    typedef function<return_type(Variant const &, Variant const &, Variant const &, Variant const &)> func4_type;
    typedef function<return_type(Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> func5_type;
    typedef function<return_type(Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> func6_type;
    typedef function<return_type(Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> func7_type;
    typedef function<return_type(Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> func8_type;
    typedef function<return_type(Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> func9_type;

    // 实现的成员函数
    classfunc_type classfunc;

    // 实现的静态函数
    func_type func;

    void declare_in(Context &) const;

    void declare_in(Context &, Class const &) const;
};

class Field {
public:
    template<typename T>
    Field(string const &_name, T const &_v) {
        name = _name;
        val = make_shared<Variant>(_v);
    }

    explicit Field(string const &_name) {
        name = _name;
    }

    // 值名称
    string name;

    // 默认数据
    value_type val;

    void declare_in(Context &) const;

    void declare_in(Context &, Class const &) const;
};

class Singleton {
public:
    typedef function<void()> func_type;

    string name;
    func_type init, fini;

    inline bool empty() const {
        return name.empty();
    }

    void declare_in(Context &) const;
};

NLUA_CLASS_PREPARE(Class)

class Class {
NLUA_CLASS_DECL(Class)

public:
    Class();

    ~Class();

    string name;

    typedef shared_ptr<Field> field_type;
    typedef map<string, field_type> fields_type;
    typedef shared_ptr<Function> function_type;
    typedef map<string, function_type> functions_type;
    typedef vector<function_type> initfunctions_type;

    // 定义构造函数
    Class &init(Function::basefunc_type, size_t args);

    Class &init(Function::basefunc0_type);

    Class &init(Function::basefunc1_type);

    Class &init(Function::basefunc2_type);

    Class &init(Function::basefunc3_type);

    Class &init(Function::basefunc4_type);

    Class &init(Function::basefunc5_type);

    Class &init(Function::basefunc6_type);

    Class &init(Function::basefunc7_type);

    Class &init(Function::basefunc8_type);

    Class &init(Function::basefunc9_type);

    // 定义析构函数
    Class &fini(Function::basefunc0_type);

    // 添加成员函数
    Class &add(string const &, Function::classfunc_type);

    Class &add(string const &, Function::classfunc0_type);

    Class &add(string const &, Function::classfunc1_type);

    Class &add(string const &, Function::classfunc2_type);

    Class &add(string const &, Function::classfunc3_type);

    Class &add(string const &, Function::classfunc4_type);

    Class &add(string const &, Function::classfunc5_type);

    Class &add(string const &, Function::classfunc6_type);

    Class &add(string const &, Function::classfunc7_type);

    Class &add(string const &, Function::classfunc8_type);

    Class &add(string const &, Function::classfunc9_type);

    // 添加静态函数
    Class &add_static(string const &, Function::func_type);

    Class &add_static(string const &, Function::func0_type);

    Class &add_static(string const &, Function::func1_type);

    Class &add_static(string const &, Function::func2_type);

    Class &add_static(string const &, Function::func3_type);

    Class &add_static(string const &, Function::func4_type);

    Class &add_static(string const &, Function::func5_type);

    Class &add_static(string const &, Function::func6_type);

    Class &add_static(string const &, Function::func7_type);

    Class &add_static(string const &, Function::func8_type);

    Class &add_static(string const &, Function::func9_type);

    // 添加成员变量
    Class &add(field_type const &);

    fields_type const &fields() const;

    functions_type const &functions() const;

    // 设置为单件模式，名称为 name 和 free_name
    Class &singleton(string const &_name, Singleton::func_type _init = nullptr, Singleton::func_type _fini = nullptr);

    typedef ::std::vector<Any> supers_type;

    // 继承，多次调用则为多继承
    Class &inherit(Any const &par);

    // 多继承
    Class &inherit(::std::initializer_list<Any> const &pars);

    // 直接声明
    void declare_in(Context &) const;

    // 声明到模块中
    void declare_in(Context &, Module const &) const;
};

NLUA_CLASS_PREPARE(Module)

class Module {
NLUA_CLASS_DECL(Module)

public:
    Module();

    ~Module();

    string name;

    typedef shared_ptr<Class> class_type;
    typedef map<string, class_type> classes_type;

    // 将类添加到模块中
    Module &add(class_type &);

    classes_type const &classes() const;

    // 声明模块
    void declare_in(Context &) const;

    // 合并其他模块
    void merge(Module const &r);
};

// 映射C++的实例

NLUA_CLASS_PREPARE(Object)

class Object {
NLUA_CLASS_DECL(Object)

    friend class Context;

    friend class ClassPrivate;

    friend class FunctionPrivate;

public:

    Object();

    ~Object();

    // 获得隐含的指针地址
    void *payload() const;

    // 设置隐含的指针地址
    void payload(void *);

    // 获得数据
    return_type get(string const &name);

    // 设置数据
    void set(string const &, value_type const &);

    template<typename T>
    void set(string const &name, T const &v) {
        set(name, make_shared<Variant>(v));
    }

    // 浅层复制
    void setfor(Object &) const;

    // 是否存在
    bool has(string const &) const;

    // 是否为空
    bool isnull() const;

    // 提升生命期
    void grab();

    // 释放
    bool drop();

    // 执行函数
    return_type invoke(string const &name, args_type const &);

    return_type invoke(string const &name);

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
};

NLUA_END

#endif
