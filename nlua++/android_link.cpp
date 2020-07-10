#include <jni.h>
#include <nlua++/nlua++.hpp>
#define __NLUA_PRIVATE__
#include <nlua++/android-prv.hpp>
#include <ajni++/ajni++.hpp>

USE_NLUA;
USE_AJNI;

#define NLUA_FUNC(cls, name) AJNI_FUNCNAME(com_nnt_nlua, cls, name)

AJNI_API(void)
NLUA_FUNC(Context, jni_1create)(JNIEnv* env, jobject thiz)
{
    Context::shared().create();
}

AJNI_API(jboolean)
NLUA_FUNC(Context, jni_1loadfile)(JNIEnv* env, jobject thiz, jstring file)
{
    return Context::shared().load(Variant(file).toString());
}

AJNI_API(jboolean)
NLUA_FUNC(Context, jni_1loadbuffer)(JNIEnv* env, jobject thiz, jbyteArray arr)
{
    auto bytes = JObject::Extract(arr)->toArray()->toBytes();
    auto const sz = bytes->size();
    if (!sz)
    {
        LOG_WARN("读取空缓存");
        return true;
    }
    return Context::shared().load(&bytes->at(0), sz);
}

AJNI_API(jint)
NLUA_FUNC(Context, jni_1global)(JNIEnv* env, jobject thiz, jstring keypath)
{
    auto r = Context::shared().global(JVariant(keypath).toString());
    if (!r)
        return -1;
    // 需要将c++的实体保存到全局再返回给java
    return gs_objects.add(r);
}

AJNI_API(void)
NLUA_FUNC(Object, jni_1finalize)(JNIEnv* env, jobject thiz, jint idx)
{
    gs_objects.remove(idx);
}

#define NLUA_OBJECT_CALL_BEGIN \
    auto fnd = gs_objects.get(idx); \
if (!fnd) { \
LOG_ERROR("没有找到LUA对象"); \
return nullptr; \
} \
auto r = JObject::Putin(toAjni(

#define NLUA_OBJECT_CALL_END \
)); \
return r ? r->asReturn() : nullptr;

AJNI_API(jobject)
NLUA_FUNC(Object, jni_1call0)(JNIEnv* env, jobject thiz, jint idx)
{
    auto fnd = gs_objects.get(idx);
    if (!fnd)
    {
        LOG_ERROR("没有找到LUA对象");
        return nullptr;
    }
    auto r = JObject::Putin(toAjni(fnd->call()));
    return r ? r->asReturn() : nullptr;
}

AJNI_API(jobject)
NLUA_FUNC(Object, jni_1call1)(JNIEnv* env, jobject thiz, jint idx, jobject p0)
{
    NLUA_OBJECT_CALL_BEGIN
        fnd->call(*fromAjni(JObject::Extract(p0)))
    NLUA_OBJECT_CALL_END
}

AJNI_API(jobject)
NLUA_FUNC(Object, jni_1call2)(JNIEnv* env, jobject thiz, jint idx, jobject p0, jobject p1)
{
    NLUA_OBJECT_CALL_BEGIN
        fnd->call(*fromAjni(JObject::Extract(p0)), *fromAjni(JObject::Extract(p1)))
    NLUA_OBJECT_CALL_END
}

AJNI_API(jobject)
NLUA_FUNC(Object, jni_1call3)(JNIEnv* env,
    jobject thiz,
    jint idx,
    jobject p0,
    jobject p1,
    jobject p2)
{
    NLUA_OBJECT_CALL_BEGIN
        fnd->call(*fromAjni(JObject::Extract(p0)),
            *fromAjni(JObject::Extract(p1)),
            *fromAjni(JObject::Extract(p2)))
    NLUA_OBJECT_CALL_END
}

AJNI_API(jobject)
NLUA_FUNC(Object, jni_1call4)(JNIEnv* env,
    jobject thiz,
    jint idx,
    jobject p0,
    jobject p1,
    jobject p2,
    jobject p3)
{
    NLUA_OBJECT_CALL_BEGIN
        fnd->call(*fromAjni(JObject::Extract(p0)),
            *fromAjni(JObject::Extract(p1)),
            *fromAjni(JObject::Extract(p2)),
            *fromAjni(JObject::Extract(p3)))
    NLUA_OBJECT_CALL_END
}

AJNI_API(jobject)
NLUA_FUNC(Object, jni_1call5)(JNIEnv* env,
    jobject thiz,
    jint idx,
    jobject p0,
    jobject p1,
    jobject p2,
    jobject p3,
    jobject p4)
{
    NLUA_OBJECT_CALL_BEGIN
        fnd->call(*fromAjni(JObject::Extract(p0)),
            *fromAjni(JObject::Extract(p1)),
            *fromAjni(JObject::Extract(p2)),
            *fromAjni(JObject::Extract(p3)),
            *fromAjni(JObject::Extract(p4)))
    NLUA_OBJECT_CALL_END
}

AJNI_API(jobject)
NLUA_FUNC(Object, jni_1call6)(JNIEnv* env,
    jobject thiz,
    jint idx,
    jobject p0,
    jobject p1,
    jobject p2,
    jobject p3,
    jobject p4,
    jobject p5)
{
    NLUA_OBJECT_CALL_BEGIN
        fnd->call(*fromAjni(JObject::Extract(p0)),
            *fromAjni(JObject::Extract(p1)),
            *fromAjni(JObject::Extract(p2)),
            *fromAjni(JObject::Extract(p3)),
            *fromAjni(JObject::Extract(p4)),
            *fromAjni(JObject::Extract(p5)))
    NLUA_OBJECT_CALL_END
}

AJNI_API(jobject)
NLUA_FUNC(Object, jni_1call7)(JNIEnv* env,
    jobject thiz,
    jint idx,
    jobject p0,
    jobject p1,
    jobject p2,
    jobject p3,
    jobject p4,
    jobject p5,
    jobject p6)
{
    NLUA_OBJECT_CALL_BEGIN
        fnd->call(*fromAjni(JObject::Extract(p0)),
            *fromAjni(JObject::Extract(p1)),
            *fromAjni(JObject::Extract(p2)),
            *fromAjni(JObject::Extract(p3)),
            *fromAjni(JObject::Extract(p4)),
            *fromAjni(JObject::Extract(p5)),
            *fromAjni(JObject::Extract(p6)))
    NLUA_OBJECT_CALL_END
}

AJNI_API(jobject)
NLUA_FUNC(Object, jni_1call8)(JNIEnv* env,
    jobject thiz,
    jint idx,
    jobject p0,
    jobject p1,
    jobject p2,
    jobject p3,
    jobject p4,
    jobject p5,
    jobject p6,
    jobject p7)
{
    NLUA_OBJECT_CALL_BEGIN
        fnd->call(*fromAjni(JObject::Extract(p0)),
            *fromAjni(JObject::Extract(p1)),
            *fromAjni(JObject::Extract(p2)),
            *fromAjni(JObject::Extract(p3)),
            *fromAjni(JObject::Extract(p4)),
            *fromAjni(JObject::Extract(p5)),
            *fromAjni(JObject::Extract(p6)),
            *fromAjni(JObject::Extract(p7)))
    NLUA_OBJECT_CALL_END
}

AJNI_API(jobject)
NLUA_FUNC(Object, jni_1call9)(JNIEnv* env,
    jobject thiz,
    jint idx,
    jobject p0,
    jobject p1,
    jobject p2,
    jobject p3,
    jobject p4,
    jobject p5,
    jobject p6,
    jobject p7,
    jobject p8)
{
    NLUA_OBJECT_CALL_BEGIN
        fnd->call(*fromAjni(JObject::Extract(p0)),
            *fromAjni(JObject::Extract(p1)),
            *fromAjni(JObject::Extract(p2)),
            *fromAjni(JObject::Extract(p3)),
            *fromAjni(JObject::Extract(p4)),
            *fromAjni(JObject::Extract(p5)),
            *fromAjni(JObject::Extract(p6)),
            *fromAjni(JObject::Extract(p7)),
            *fromAjni(JObject::Extract(p8)))
    NLUA_OBJECT_CALL_END
}
