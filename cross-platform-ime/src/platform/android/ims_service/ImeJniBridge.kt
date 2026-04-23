package com.ime.crossplatform

/**
 * JNI桥接层
 * 连接Kotlin/Java层与C++核心引擎
 */
object ImeJniBridge {

    init {
        System.loadLibrary("ime_core_jni")
    }

    @JvmStatic
    external fun createEngine(): Long

    @JvmStatic
    external fun destroyEngine(engine: Long)

    @JvmStatic
    external fun processInput(engine: Long, input: String): Int

    @JvmStatic
    external fun getCandidates(engine: Long, input: String): Array<CrossPlatformIME.Candidate>?

    @JvmStatic
    external fun commitCandidate(engine: Long, text: String, code: String)

    @JvmStatic
    external fun initializeDictionary(engine: Long, path: String): Boolean

    @JvmStatic
    external fun loadPlugin(engine: Long, path: String): Boolean

    @JvmStatic
    external fun setConfig(engine: Long, key: String, value: String): Boolean

    @JvmStatic
    external fun getConfig(engine: Long, key: String): String?

    @JvmStatic
    external fun syncDictionary(engine: Long, serverUrl: String, token: String): Int
}