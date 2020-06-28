package com.nnt.nlua

class Context {

    fun create() {
        jni_create()
    }

    private external fun jni_create()

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
