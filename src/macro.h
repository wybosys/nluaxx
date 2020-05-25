#ifndef __NNT_MACROXX_H_INCLUDED
#define __NNT_MACROXX_H_INCLUDED

// github.com/wybosys/nnt.macro

#define NNT_FRIEND(cls) friend class cls;
#define NNT_NOCOPY(cls) \
private:                 \
    cls(cls &) = delete;
#define NNT_PRIVATECLASS(cls) cls##Private

#define NNT_CLASS_PREPARE(cls) \
    class cls;                  \
    class NNT_PRIVATECLASS(cls);

#define NNT_CLASS_DECL(cls)                                         \
protected:                                                           \
    typedef NNT_PRIVATECLASS(cls) private_class_type;               \
    friend class NNT_PRIVATECLASS(cls);                             \
    private_class_type *d_ptr = nullptr;                             \
    friend private_class_type *nnt::DPtr<cls, private_class_type>(cls *); \
                                                                     \
private:                                                             \
    NNT_NOCOPY(cls);

#define NNT_CLASS_CONSTRUCT(...) \
    d_ptr = new private_class_type(__VA_ARGS__);
#define NNT_CLASS_DESTORY() \
    delete d_ptr;            \
    d_ptr = nullptr;

#define NNT_SINGLETON_DECL(cls) \
public:                          \
    static cls &shared(); \
    static bool is_shared(); \
    static void free_shared();

#define NNT_SINGLETON_IMPL(cls, ...)       \
    static cls *_shared = nullptr;          \
    cls &cls::shared()                      \
    {                                       \
        if (_shared == nullptr)             \
        {                                   \
            _shared = new cls(__VA_ARGS__); \
        }                                   \
        return *_shared;                    \
    } \
    bool cls::is_shared() { return _shared != nullptr; } \
    void cls::free_shared() { if (_shared) { delete _shared; _shared = nullptr; } }

#define __NNT_RAW(L) L
#define __NNT_COMBINE(L, R) L##R
#define _NNT_COMBINE(L, R) __NNT_COMBINE(L, R)

#define NNT_AUTOGUARD(obj, ...) ::std::lock_guard<::std::mutex> _NNT_COMBINE(__auto_guard_, __LINE__)(obj);

#if defined(__ANDROID__) || defined(__APPLE__)
#define NNT_ISMOBILE 1
#define NNT_ISSHELL 0
#else
#define NNT_ISMOBILE 0
#define NNT_ISSHELL 1
#endif

namespace nnt
{
    template <class T, class TP = typename T::private_class_type>
    static TP* DPtr(T* obj)
    {
        return obj->d_ptr;
    }
}

#endif