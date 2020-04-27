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
typedef ::std::initializer_list<Variant> args_type;
typedef shared_ptr<Object> self_type;

class Any {
public:

    Any(shared_ptr<Class> const &r) :
            clazz(r) {}

    Any(shared_ptr<Object> const &r) :
            object(r) {}

    shared_ptr<Class> clazz;
    shared_ptr<Object> object;
};

NLUA_CLASS_PREPARE(Context);

class Context {
NLUA_CLASS_DECL(Context);

public:

    Context();

    ~Context();

    // 加载文件
    bool load(path const &);

    // 执行函数
    return_type invoke(string const &fname, args_type const &args = {});

    // lua库的目录
    path libraries_path;

    // 获得全局对象
    self_type global(string const &);
};

NLUA_CLASS_PREPARE(Function)

class Function {
NLUA_CLASS_DECL(Function)

public:

    Function();

    ~Function();

    string name;

    // 类函数
    typedef ::std::function<return_type(self_type &self, args_type const &)> classfunc_type;

    typedef ::std::function<return_type(self_type &self)> classfunc0_type;
    typedef ::std::function<return_type(self_type &self, Variant const &)> classfunc1_type;
    typedef ::std::function<return_type(self_type &self, Variant const &, Variant const &)> classfunc2_type;
    typedef ::std::function<return_type(self_type &self, Variant const &, Variant const &, Variant const &)> classfunc3_type;
    typedef ::std::function<return_type(self_type &self, Variant const &, Variant const &, Variant const &, Variant const &)> classfunc4_type;
    typedef ::std::function<return_type(self_type &self, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> classfunc5_type;
    typedef ::std::function<return_type(self_type &self, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> classfunc6_type;
    typedef ::std::function<return_type(self_type &self, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> classfunc7_type;
    typedef ::std::function<return_type(self_type &self, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> classfunc8_type;
    typedef ::std::function<return_type(self_type &self, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> classfunc9_type;

    // 静态函数
    typedef ::std::function<return_type(args_type const &)> func_type;

    typedef ::std::function<return_type()> func0_type;
    typedef ::std::function<return_type(Variant const &)> func1_type;
    typedef ::std::function<return_type(Variant const &, Variant const &)> func2_type;
    typedef ::std::function<return_type(Variant const &, Variant const &, Variant const &)> func3_type;
    typedef ::std::function<return_type(Variant const &, Variant const &, Variant const &, Variant const &)> func4_type;
    typedef ::std::function<return_type(Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> func5_type;
    typedef ::std::function<return_type(Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> func6_type;
    typedef ::std::function<return_type(Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> func7_type;
    typedef ::std::function<return_type(Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> func8_type;
    typedef ::std::function<return_type(Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &, Variant const &)> func9_type;

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

    Field(string const &_name) {
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
    typedef ::std::function<void()> func_type;

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
    typedef ::std::map<string, field_type> fields_type;
    typedef shared_ptr<Function> function_type;
    typedef ::std::map<string, function_type> functions_type;

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

    [[nodiscard]]
    inline fields_type const &fields() const {
        return _fields;
    }

    [[nodiscard]]
    inline functions_type const &functions() const {
        return _functions;
    }

    // 设置为单件模式，名称为 name 和 free_name
    inline Class &singleton(string const &_name, Singleton::func_type _init = nullptr, Singleton::func_type _fini = nullptr) {
        _singleton.name = _name;
        _singleton.init = ::std::move(_init);
        _singleton.fini = ::std::move(_fini);
        return *this;
    }

    typedef ::std::vector<Any> supers_type;

    // 继承，多次调用则为多继承
    inline Class &inherit(Any const &par) {
        _supers.emplace_back(par);
        return *this;
    }

    // 多继承
    inline Class &inherit(::std::initializer_list<Any> const &pars) {
        for (auto &e:pars) {
            _supers.emplace_back(e);
        }
        return *this;
    }


    void declare_in(Context &) const;

    void declare_in(Context &, Module const &) const;

private:
    Singleton _singleton;
    fields_type _fields;
    functions_type _functions;
    supers_type _supers;
};

class Module {
public:

    string name;

    typedef shared_ptr<Class> class_type;
    typedef ::std::map<string, class_type> classes_type;

    Module &add_class(class_type &);

    [[nodiscard]]
    inline classes_type const &classes() const {
        return _classes;
    }

    void declare_in(Context &) const;

private:
    classes_type _classes;
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
    void *pointer() const;

    inline operator void *() const {
        return pointer();
    }

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
