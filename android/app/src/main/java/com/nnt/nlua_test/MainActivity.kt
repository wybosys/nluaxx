package com.nnt.nlua_test

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import com.getkeepsafe.relinker.ReLinker

class MainActivity() : AppCompatActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // 加载so
        ReLinker.loadLibrary(this, "nlua")
    }
}
