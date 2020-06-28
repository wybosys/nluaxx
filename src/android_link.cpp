#include <jni.h>
#include "nlua++.hpp"

USE_NLUA;

extern "C" JNIEXPORT void JNICALL
Java_com_nnt_nlua_Context_jni_1create(JNIEnv *env, jobject thiz)
{
    Context::shared().create();
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_nnt_nlua_Context_jni_1loadfile(JNIEnv *env, jobject thiz, jstring file)
{
    jboolean copied;
    char const *str = env->GetStringUTFChars(file, &copied);
    bool r = Context::shared().load(string(str));
    if (copied)
        env->ReleaseStringUTFChars(file, str);
    return r;
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_nnt_nlua_Context_jni_1loadbuffer(JNIEnv *env, jobject thiz, jbyteArray arr)
{
    size_t len = env->GetArrayLength(arr);
    jboolean copied;
    void *str = env->GetByteArrayElements(arr, &copied);
    bool r = Context::shared().load(str, len);
    if (copied)
        env->ReleaseByteArrayElements(arr, (jbyte *) str, 0);
    return r;
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

extern "C" JNIEXPORT jint JNICALL
Java_com_nnt_nlua_Context_jni_1global(JNIEnv *env, jobject thiz, jstring keypath)
{
    /*
    jboolean copied;
    char const *str = env->GetStringUTFChars(keypath, &copied);
    auto r = Context::shared().global(str);
    if (copied)
        env->ReleaseStringUTFChars(keypath, str);
    if (!r)
        return -1;
    // 需要将c++的实体保存到全局再返回给java
    return gs_objects.add(r);
     */
    return -1;
}

extern "C" JNIEXPORT void JNICALL
Java_com_nnt_nlua_Object_jni_1finalize(JNIEnv *env, jobject thiz, jint idx)
{
    //gs_objects.remove(idx);
}
