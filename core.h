#ifndef __NLUA_H_INCLUDED
#define __NLUA_H_INCLUDED

#ifndef NLUA_NS
#define NLUA_NS nlua
#endif

#define NLUA_BEGIN namespace NLUA_NS {
#define NLUA_END }
#define USE_NLUA using namespace NLUA_NS;

#include <string>
#include <memory>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <functional>

NLUA_BEGIN

// 执行main循环
extern int Exec();

using ::std::string;
using ::std::unique_ptr;
using ::std::shared_ptr;
using ::std::weak_ptr;
using ::std::make_shared;
using ::std::make_pair;
using ::std::cin;
using ::std::cout;
using ::std::endl;
using ::std::cerr;
using ::std::exception;
using ::std::stringstream;

typedef long long longlong;
typedef unsigned long long ulonglong;
typedef longlong integer;
typedef ulonglong uinteger, pointer;
typedef double number;

#define NLUA_FRIEND(cls) friend class cls;
#define NLUA_NOCOPY(cls) private: cls(cls&) = delete;
#define NLUA_PRIVATECLASS(cls) cls##Private

#define NLUA_CLASS_PREPARE(cls) \
class cls; \
class NLUA_PRIVATECLASS(cls);

#define NLUA_CLASS_DECL(cls) \
protected: typedef NLUA_PRIVATECLASS(cls) private_class_type; \
friend class NLUA_PRIVATECLASS(cls); \
private_class_type *d_ptr = nullptr; \
friend private_class_type* DPtr<cls, private_class_type>(cls*); \
private: NLUA_NOCOPY(cls);

#define NLUA_CLASS_CONSTRUCT(...) \
d_ptr = new private_class_type(__VA_ARGS__);
#define NLUA_CLASS_DESTORY() \
delete d_ptr; d_ptr = nullptr;

template<class T, class TP = typename T::private_class_type>
inline TP *DPtr(T *obj) {
    return obj->d_ptr;
}

#define NLUA_SINGLETON_DECL(cls) \
public: static cls& shared();
#define NLUA_SINGLETON_IMPL(cls) \
static cls* _shared = nullptr; \
cls& cls::shared() { \
if (_shared == nullptr) { \
_shared = new cls(); \
} \
return *_shared; \
}

#define __NLUA_RAW(L) L
#define __NLUA_COMBINE(L, R) L##R
#define _NLUA_COMBINE(L, R) __NLUA_COMBINE(L, R)

#define NLUA_AUTOGUARD(obj, ...) ::std::lock_guard<::std::mutex> _NLUA_COMBINE(__auto_guard_, __LINE__)(obj);

class error : public exception {
public:

    explicit error(int code, string const &msg = "") : _code(code), _msg(msg) {}

    virtual const char *what() const throw() {
        return _msg.c_str();
    }

private:
    int _code;
    string _msg;
};

NLUA_END

#endif
