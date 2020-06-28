#include <jni.h>
#include "nlua++.hpp"

USE_NLUA;

extern "C" void Java_com_nnt_nlua_Context_jni_1create(JNIEnv *env, jobject thiz)
{
    Context::shared().create();
}

extern "C" jboolean Java_com_nnt_nlua_Context_jni_1loadfile(JNIEnv *env, jobject thiz, jstring file)
{
    jboolean copied;
    char const *str = env->GetStringUTFChars(file, &copied);
    bool r = Context::shared().load(string(str));
    if (copied)
        env->ReleaseStringUTFChars(file, str);
    return r;
}

extern "C" jboolean
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
