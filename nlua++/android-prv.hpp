#ifndef __NLUA_ANDROID_H_INCLUDED
#define __NLUA_ANDROID_H_INCLUDED

#ifndef __NLUA_PRIVATE__
#error "禁止在外部引用该文件"
#endif

#include <ajni++/ajni++.hpp>
#include "variant.hpp"

NLUA_BEGIN

template<typename T>
class GsObject
{
public:

    GsObject()
        : _cnt(0)
    {}

    typedef shared_ptr<T> object_type;

    size_t add(object_type const &v)
    {
        NNT_AUTOGUARD(_mtx);
        size_t idx = ++_cnt;
        _map[idx] = v;
        return idx;
    }

    void remove(size_t idx)
    {
        NNT_AUTOGUARD(_mtx);
        _map.erase(idx);
    }

    object_type get(size_t idx)
    {
        NNT_AUTOGUARD(_mtx);
        auto fnd = _map.find(idx);
        return fnd == _map.end() ? nullptr : fnd->second;
    }

private:
    ::std::map<size_t, object_type> _map;
    ::std::atomic<size_t> _cnt;
    ::std::mutex _mtx;
};

extern GsObject<Object> gs_objects;

// 从lua对象转换为android对象
extern shared_ptr<::AJNI_NS::JVariant> toAjni(shared_ptr < Variant >

const&);

// 从android对象转换lua对象
extern shared_ptr<Variant> fromAjni(shared_ptr < ::AJNI_NS::JVariant >

const&);

NLUA_END

#endif
