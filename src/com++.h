#ifndef __COMXX_H_INCLUDED
#define __COMXX_H_INCLUDED

// github.com/wybosys/nnt.comxx

#define COMXX_NS com
#define COMXX_BEGIN    \
    namespace COMXX_NS \
    {
#define COMXX_END }

#include <atomic>
#include <string>
#include <memory>
#include <functional>
#include <initializer_list>
#include <vector>
#include <map>

COMXX_BEGIN

using namespace ::std;

typedef struct
{
    unsigned long d1;
    unsigned short d2;
    unsigned short d3;
    union {
        unsigned char d1[8];
        struct
        {
            unsigned int d1;
            unsigned int d2;
        } d2;
    } d4;
} IID;

static bool operator<(IID const &l, IID const &r)
{
    return l.d1 < r.d1 || l.d2 < r.d2 || l.d3 < r.d3 || l.d4.d2.d1 < r.d4.d2.d1 || l.d4.d2.d2 < r.d4.d2.d2;
}

class Variant
{
public:
    typedef vector<unsigned char> bytes_t;
    typedef function<Variant(initializer_list<Variant> const &)> func_t;

    enum struct VT
    {
        NIL = 0,
        INT = 1,
        UINT = 2,
        LONG = 3,
        ULONG = 4,
        SHORT = 5,
        USHORT = 6,
        LONGLONG = 7,
        ULONGLONG = 8,
        FLOAT = 9,
        DOUBLE = 10,
        CHAR = 11,
        UCHAR = 12,
        BYTES = 13,
        STRING = 14,
        FUNCTION = 15,
        OBJECT = 16,
        BOOLEAN = 17,
        POINTER = 18,
    };

    Variant();
    Variant(class IObject *);
    Variant(void*);
    Variant(nullptr_t);
    Variant(int);
    Variant(unsigned int);
    Variant(long);
    Variant(unsigned long);
    Variant(short);
    Variant(unsigned short);
    Variant(long long);
    Variant(unsigned long long);
    Variant(float);
    Variant(double);
    Variant(char);
    Variant(unsigned char);
    Variant(bool);
    Variant(bytes_t const &);
    Variant(string const &);
    Variant(char const*);
    Variant(func_t const &);
    ~Variant();

    const VT vt;

    operator class IObject *() const;
    operator void*() const;
    operator int() const;
    operator unsigned int() const;
    operator long() const;
    operator unsigned long() const;
    operator short() const;
    operator unsigned short() const;
    operator long long() const;
    operator unsigned long long() const;
    operator float() const;
    operator double() const;
    operator char() const;
    operator unsigned char() const;
    operator bool() const;
    operator bytes_t const &() const;
    operator string const &() const;
    operator func_t const &() const;

private:
    union {
        class IObject *o;
        void *p;
        int i;
        unsigned int ui;
        long l;
        unsigned long ul;
        short s;
        unsigned short us;
        long long ll;
        unsigned long long ull;
        float f;
        double d;
        char c;
        unsigned char uc;
        bool b;
    } _pod = {0};
    shared_ptr<bytes_t> _bytes;
    shared_ptr<string> _str;
    shared_ptr<func_t> _func;
};

typedef initializer_list<Variant> args_t;

#define COMXX_PPARGS_0(args)
#define COMXX_PPARGS_1(args) *args.begin()
#define COMXX_PPARGS_2(args) COMXX_PPARGS_1(args), *(args.begin() + 1)
#define COMXX_PPARGS_3(args) COMXX_PPARGS_2(args), *(args.begin() + 2)
#define COMXX_PPARGS_4(args) COMXX_PPARGS_3(args), *(args.begin() + 3)
#define COMXX_PPARGS_5(args) COMXX_PPARGS_4(args), *(args.begin() + 4)
#define COMXX_PPARGS_6(args) COMXX_PPARGS_5(args), *(args.begin() + 5)
#define COMXX_PPARGS_7(args) COMXX_PPARGS_6(args), *(args.begin() + 6)
#define COMXX_PPARGS_8(args) COMXX_PPARGS_7(args), *(args.begin() + 7)
#define COMXX_PPARGS_9(args) COMXX_PPARGS_8(args), *(args.begin() + 8)
#define COMXX_PPARGS_10(args) COMXX_PPARGS_9(args), *(args.begin() + 9)

class IObject
{
public:
    IObject() = default;
    virtual ~IObject() = default;

    virtual void grab() const;
    virtual bool drop() const;

    virtual Variant query(IID const &) const = 0;

private:
    mutable atomic_long _referenceCount = 1;
};

template <typename T>
class shared_ref
{
public:
    shared_ref(T *obj) : _ptr(obj)
    {
        if (_ptr)
            _ptr->grab();
    }

    shared_ref(shared_ref const &r) : _ptr(r._ptr)
    {
        if (_ptr)
            _ptr->grab();
    }

    ~shared_ref()
    {
        if (_ptr)
        {
            _ptr->drop();
            _ptr = nullptr;
        }
    }

    inline T *operator->()
    {
        return _ptr;
    }

    inline T const *operator->() const
    {
        return _ptr;
    }

private:
    T *_ptr = nullptr;
};

template <typename T, class... Args>
inline shared_ref<T> make_shared_ref(Args &&... args)
{
    T *obj = new T(forward<Args>(args)...);
    shared_ref<T> r(obj);
    obj->drop();
    return r;
}

#define COMXX_DEFINE(name) \
    static const COMXX_NS::IID name

#define COMXX_IID(name) \
    const COMXX_NS::IID name

COMXX_DEFINE(IID_NEW) = {0xd0733610, 0x6360, 0x4362, {0xb9, 0x42, 0xf4, 0xdb, 0xd2, 0x25, 0x69, 0xf4}};
COMXX_DEFINE(IID_CLONE) = {0x3e670b35, 0xdd3e, 0x4530, {0xb2, 0xff, 0x77, 0x12, 0xb3, 0x5d, 0xf4, 0x93}};

class CustomObject : public IObject
{
public:
    CustomObject() = default;
    virtual ~CustomObject()
    {
        clear();
    }

    typedef struct
    {
        string name;
        Variant::func_t func;
    } func_t;

    typedef map<IID, IObject *> objects_t;
    typedef map<IID, func_t> functions_t;

    virtual Variant query(IID const &) const;

    virtual void clear();

    virtual void add(IID const &, IObject *);

    virtual void add(IID const &, string const &name, Variant::func_t);

    functions_t const& functions() const;

private:
    objects_t _objects;
    functions_t _functions;
};

inline Variant::Variant(IObject *v) : vt(VT::OBJECT)
{
    _pod.o = v;
    if (v)
    {
        v->grab();
    }
}

inline Variant::Variant() :vt(VT::NIL) {}
inline Variant::Variant(void* v) :vt(VT::POINTER) { _pod.p = v; }
inline Variant::Variant(nullptr_t) : vt(VT::POINTER) { _pod.p = nullptr; }
inline Variant::Variant(int v) : vt(VT::INT) { _pod.i = v; }
inline Variant::Variant(unsigned int v) : vt(VT::UINT) { _pod.ui = v; }
inline Variant::Variant(long v) : vt(VT::LONG) { _pod.l = v; }
inline Variant::Variant(unsigned long v) : vt(VT::ULONG) { _pod.ul = v; }
inline Variant::Variant(short v) : vt(VT::SHORT) { _pod.s = v; }
inline Variant::Variant(unsigned short v) : vt(VT::USHORT) { _pod.us = v; }
inline Variant::Variant(long long v) : vt(VT::LONGLONG) { _pod.ll = v; }
inline Variant::Variant(unsigned long long v) : vt(VT::ULONGLONG) { _pod.ull = v; }
inline Variant::Variant(float v) : vt(VT::FLOAT) { _pod.f = v; }
inline Variant::Variant(double v) : vt(VT::DOUBLE) { _pod.d = v; }
inline Variant::Variant(char v) : vt(VT::CHAR) { _pod.c = v; }
inline Variant::Variant(unsigned char v) : vt(VT::UCHAR) { _pod.uc = v; }
inline Variant::Variant(bool v) : vt(VT::BOOLEAN) { _pod.b = v; }
inline Variant::Variant(bytes_t const &v) : vt(VT::BYTES) { _bytes = make_shared<bytes_t>(v); }
inline Variant::Variant(string const &v) : vt(VT::STRING) { _str = make_shared<string>(v); }
inline Variant::Variant(char const* v) : vt(VT::STRING) { _str = make_shared<string>(v); }
inline Variant::Variant(func_t const &v) : vt(VT::FUNCTION) { _func = make_shared<func_t>(v); }

inline Variant::~Variant()
{
    if (vt == VT::OBJECT && _pod.o)
    {
        _pod.o->drop();
        _pod.o = nullptr;
    }
}

inline Variant::operator class IObject *() const { return _pod.o; }
inline Variant::operator void *() const { return _pod.p; }
inline Variant::operator int() const { return _pod.i; }
inline Variant::operator unsigned int() const { return _pod.ui; }
inline Variant::operator long() const { return _pod.l; }
inline Variant::operator unsigned long() const { return _pod.ul; }
inline Variant::operator short() const { return _pod.s; }
inline Variant::operator unsigned short() const { return _pod.us; }
inline Variant::operator long long() const { return _pod.ll; }
inline Variant::operator unsigned long long() const { return _pod.ull; }
inline Variant::operator float() const { return _pod.f; }
inline Variant::operator double() const { return _pod.d; }
inline Variant::operator char() const { return _pod.c; }
inline Variant::operator unsigned char() const { return _pod.uc; }
inline Variant::operator bool() const { return _pod.b; }
inline Variant::operator bytes_t const &() const { return *_bytes; }
inline Variant::operator string const &() const { return *_str; }
inline Variant::operator func_t const &() const { return *_func; }

inline void IObject::grab() const
{
    _referenceCount += 1;
}

inline bool IObject::drop() const
{
    if (--_referenceCount == 0)
    {
        delete this;
        return true;
    }
    return false;
}

inline Variant CustomObject::query(IID const &iid) const
{
    {
        auto fnd = _objects.find(iid);
        if (fnd != _objects.end())
            return fnd->second;
    }

    {
        auto fnd = _functions.find(iid);
        if (fnd != _functions.end())
            return fnd->second.func;
    }

    return NULL;
}

inline void CustomObject::clear()
{
    for (auto &e : _objects)
    {
        e.second->drop();
    }
    _objects.clear();
    _functions.clear();
}

inline void CustomObject::add(IID const &iid, IObject *obj)
{
    auto fnd = _objects.find(iid);
    if (fnd != _objects.end())
    {
        fnd->second->drop();
    }

    obj->grab();
    _objects[iid] = obj;
}

inline void CustomObject::add(IID const &iid, string const &name, Variant::func_t func)
{
    _functions[iid] = {name, func};
}

inline CustomObject::functions_t const& CustomObject::functions() const {
    return _functions;
}

template <class T>
struct function_traits
{
private:
    using call_type = function_traits<decltype(&T::operator())>;

public:
    using return_type = typename call_type::return_type;
    static const size_t count = call_type::count - 1;

    template <size_t N>
    struct argument
    {
        static_assert(N < count, "参数数量错误");
        using type = typename call_type::template argument<N + 1>::type;
    };
};

template <class R, class... Args>
struct function_traits<R(*)(Args...)> : public function_traits<R(Args...)>
{
};

template <class R, class... Args>
struct function_traits<R(Args...)>
{
    using return_type = R;
    static const size_t count = sizeof...(Args);

    template <size_t N>
    struct argument
    {
        static_assert(N < count, "参数数量错误");
        using type = typename tuple_element<N, tuple<Args...>>::type;
    };
};

template <class C, class R, class... Args>
struct function_traits<R(C::*)(Args...)> : public function_traits<R(Args...)>
{
};

template <class C, class R, class... Args>
struct function_traits<R(C::*)(Args...) const> : public function_traits<R(Args...)>
{
};

template <class C, class R>
struct function_traits<R(C::*)> : public function_traits<R(C &)>
{
};

template <class T>
struct function_traits<T&> : public function_traits<T>
{
};

template <class T>
struct function_traits<T&&> : public function_traits<T>
{
};

template <typename F, size_t N = function_traits<F>::count>
struct function_call {
    template <typename C>
    inline typename function_traits<F>::return_type operator()(F const& fn, C* self, args_t const& args) {
        return (self->*fn)();
    }
};

#define COMXX_FUNCTION_CALL(N, __args__) \
template <typename F> \
struct function_call<F, N> { \
	template <typename C> \
	inline typename function_traits<F>::return_type operator()(F const& fn, C* self, args_t const& args) { \
		return (self->*fn)(__args__); \
	} \
};

COMXX_FUNCTION_CALL(1, COMXX_PPARGS_1(args))
COMXX_FUNCTION_CALL(2, COMXX_PPARGS_2(args))
COMXX_FUNCTION_CALL(3, COMXX_PPARGS_3(args))
COMXX_FUNCTION_CALL(4, COMXX_PPARGS_4(args))
COMXX_FUNCTION_CALL(5, COMXX_PPARGS_5(args))
COMXX_FUNCTION_CALL(6, COMXX_PPARGS_6(args))
COMXX_FUNCTION_CALL(7, COMXX_PPARGS_7(args))
COMXX_FUNCTION_CALL(8, COMXX_PPARGS_8(args))
COMXX_FUNCTION_CALL(9, COMXX_PPARGS_9(args))


#define COMXX_CUSTOMOBJECT_ADD(idd, name, func) \
add(idd, #name, [&](COMXX_NS::args_t const& args)->COMXX_NS::Variant { \
return COMXX_NS::function_call<decltype(&func)>()(&func, this, args); \
	})
 
COMXX_END

#endif