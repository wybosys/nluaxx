package com.nnt.nlua

class Context {

    // 创建lua运行环境
    fun create() {
        jni_create()
    }

    private external fun jni_create()

    // 绑定lua运行环境，void* 位于 C++ 中，所以绑定也位于C++中处理
    // fun attach() {}

    // 加载文件
    fun load(file: String): Boolean {
        return jni_loadfile(file)
    }

    private external fun jni_loadfile(file: String): Boolean

    // 加载缓存
    fun load(bytes: ByteArray): Boolean {
        return jni_loadbuffer(bytes)
    }

    private external fun jni_loadbuffer(bytes: ByteArray): Boolean

    companion object {
        private var _shared: Context? = null

        val shared: Context
            get() {
                if (_shared == null) {
                    _shared = Context()
                }
                return _shared!!
            }
    }
}
