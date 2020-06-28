package com.nnt.nlua

class Object(val idx: Int) {

    // 释放C++层对应的对象
    protected fun finalize() {
        jni_finalize(idx)
    }

    private external fun jni_finalize(idx: Int)

    // 调用函数
    fun call() {
        
    }
}
