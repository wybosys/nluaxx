#include <jni.h>
#include "nlua++.hpp"

USE_NLUA;

extern "C" void Java_com_nnt_nlua_Context_jni_1create(JNIEnv *env, jobject thiz)
{
    Context::shared().create();
}
