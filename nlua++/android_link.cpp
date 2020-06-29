#include <jni.h>
#include <nlua++/nlua++.hpp>
#include <ajni++/ajni++.hpp>

USE_NLUA;
USE_AJNI;

AJNI_API(void) Java_com_nnt_nlua_Context_jni_1create(JNIEnv *env, jobject thiz)
{
    Context::shared().create();
}

AJNI_API(jboolean) Java_com_nnt_nlua_Context_jni_1loadfile(JNIEnv *env, jobject thiz,
                                                           jstring file)
{
    return Context::shared().load(Variant(file).toString());
}

AJNI_API(jboolean) Java_com_nnt_nlua_Context_jni_1loadbuffer(JNIEnv *env,
                                                             jobject thiz,
                                                             jbyteArray arr)
{
    auto bytes = JObject::Extract(arr)->toArray()->toBytes();
    return Context::shared().load(&bytes->at(0), bytes->size());
}

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

// 保存返回给Java层的C++对象
static GsObject<Object> gs_objects;

AJNI_API(jint) Java_com_nnt_nlua_Context_jni_1global(JNIEnv *env, jobject thiz, jstring keypath)
{
    auto r = Context::shared().global(JVariant(keypath).toString());
    if (!r)
        return -1;
    // 需要将c++的实体保存到全局再返回给java
    return gs_objects.add(r);
}

AJNI_API(void) Java_com_nnt_nlua_Object_jni_1finalize(JNIEnv *env, jobject thiz, jint idx)
{
    gs_objects.remove(idx);
}
