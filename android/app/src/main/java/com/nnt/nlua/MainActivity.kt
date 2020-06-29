package com.nnt.nlua

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.getkeepsafe.relinker.ReLinker

class MainActivity() : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // 加载so
        ReLinker.loadLibrary(this, "tester")

        // 测试
        Context.shared.create()

        // 加载包中的lua脚本
        val stm = resources.openRawResource(R.raw.test0)
        Context.shared.load(stm.readBytes())

        // 获得脚本中的函数
        val obj = Context.shared.global("Test0")!!
        var r = obj.call()
        r = obj.call(123)
        r = obj.call(null)
        r = 0
    }
}
