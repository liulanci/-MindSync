package com.ime.crossplatform

import android.inputmethodservice.InputMethodService
import android.view.KeyEvent
import android.view.View
import android.view.inputmethod.CompletionInfo
import android.view.inputmethod.InputConnection
import android.widget.LinearLayout
import android.widget.HorizontalScrollView
import android.widget.TextView
import android.graphics.Color
import android.graphics.Typeface

/**
 * Android输入法服务
 * 支持Android 8.0 (API 26)及更高版本
 */
class CrossPlatformIME : InputMethodService() {

    private var candidateContainer: LinearLayout? = null
    private var candidateScrollView: HorizontalScrollView? = null
    private var engine: Long = 0
    private var currentInput = StringBuilder()

    companion object {
        init {
            System.loadLibrary("ime_core_jni")
        }
    }

    // JNI接口
    private external fun nativeCreateEngine(): Long
    private external fun nativeDestroyEngine(engine: Long)
    private external fun nativeProcessInput(engine: Long, input: String): Array<Candidate>?
    private external fun nativeGetCandidates(engine: Long, input: String): Array<Candidate>?
    private external fun nativeCommitCandidate(engine: Long, text: String, code: String)

    override fun onCreate() {
        super.onCreate()
        engine = nativeCreateEngine()
    }

    override fun onCreateInputView(): View {
        val layout = LinearLayout(this).apply {
            orientation = LinearLayout.VERTICAL
            layoutParams = LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                LinearLayout.LayoutParams.WRAP_CONTENT
            )
            setBackgroundColor(Color.parseColor("#F5F5F5"))
        }

        candidateScrollView = HorizontalScrollView(this).apply {
            layoutParams = LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                44.dpToPx()
            )
        }

        candidateContainer = LinearLayout(this).apply {
            orientation = LinearLayout.HORIZONTAL
            layoutParams = LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.WRAP_CONTENT,
                LinearLayout.LayoutParams.MATCH_PARENT
            )
            setPadding(8.dpToPx(), 4.dpToPx(), 8.dpToPx(), 4.dpToPx())
        }

        candidateScrollView?.addView(candidateContainer)
        layout.addView(candidateScrollView)

        val toolbarLayout = createToolbarLayout()
        layout.addView(toolbarLayout)

        return layout
    }

    override fun onStartInput(attribute: android.view.inputmethod.EditorInfo?, restarting: Boolean) {
        super.onStartInput(attribute, restarting)
        currentInput.clear()
        updateCandidates()
    }

    override fun onKeyDown(keyCode: Int, event: KeyEvent?): Boolean {
        when (keyCode) {
            KeyEvent.KEYCODE_DEL -> {
                if (currentInput.isNotEmpty()) {
                    currentInput.deleteCharAt(currentInput.length - 1)
                    updateCandidates()
                    return true
                }
            }
            KeyEvent.KEYCODE_ENTER -> {
                if (currentInput.isNotEmpty()) {
                    commitCurrentInput()
                    return true
                }
            }
            KeyEvent.KEYCODE_SPACE -> {
                if (currentInput.isNotEmpty()) {
                    selectCandidate(0)
                    return true
                }
            }
        }

        return super.onKeyDown(keyCode, event)
    }

    override fun onKey(keyCode: Int, event: KeyEvent?): Boolean {
        val c = event?.unicodeChar?.toChar()
        if (c != null && c.isLetterOrDigit()) {
            currentInput.append(c)
            updateCandidates()
            return true
        }
        return false
    }

    private fun updateCandidates() {
        val container = candidateContainer ?: return

        container.removeAllViews()

        if (currentInput.isEmpty() || engine == 0L) {
            candidateScrollView?.visibility = View.GONE
            return
        }

        val candidates = nativeGetCandidates(engine, currentInput.toString())
        if (candidates.isNullOrEmpty()) {
            candidateScrollView?.visibility = View.GONE
            return
        }

        candidateScrollView?.visibility = View.VISIBLE

        candidates.forEachIndexed { index, candidate ->
            val textView = TextView(this).apply {
                text = candidate.text
                textSize = 16f
                setTypeface(null, Typeface.NORMAL)
                setTextColor(Color.parseColor("#333333"))
                setPadding(12.dpToPx(), 4.dpToPx(), 12.dpToPx(), 4.dpToPx())
                setBackgroundColor(Color.parseColor("#FFFFFF"))
                setOnClickListener { selectCandidate(index) }
            }
            container.addView(textView)
        }
    }

    private fun selectCandidate(index: Int) {
        if (engine == 0L || currentInput.isEmpty()) return

        val candidates = nativeGetCandidates(engine, currentInput.toString())
        if (candidates != null && index < candidates.size) {
            val candidate = candidates[index]
            currentInput.append(candidate.text)
            nativeCommitCandidate(engine, candidate.text, candidate.code)
            currentInput.clear()
            updateCandidates()
        }
    }

    private fun commitCurrentInput() {
        val inputConnection = currentInputConnection
        if (inputConnection != null && currentInput.isNotEmpty()) {
            inputConnection.commitText(currentInput.toString(), 1)
            currentInput.clear()
            updateCandidates()
        }
    }

    private fun createToolbarLayout(): LinearLayout {
        return LinearLayout(this).apply {
            orientation = LinearLayout.HORIZONTAL
            layoutParams = LinearLayout.LayoutParams(
                LinearLayout.LayoutParams.MATCH_PARENT,
                40.dpToPx()
            )
            setPadding(4.dpToPx(), 2.dpToPx(), 4.dpToPx(), 2.dpToPx())

            val spaceBtn = createToolbarButton("空格") {
                currentInputConnection?.commitText(" ", 1)
            }
            val deleteBtn = createToolbarButton("删除") {
                currentInputConnection?.deleteSurroundingText(1, 0)
            }
            val enterBtn = createToolbarButton("换行") {
                currentInputConnection?.commitText("\n", 1)
            }
            val modeBtn = createToolbarButton("中/英") {
                switchInputType()
            }

            addView(spaceBtn)
            addView(deleteBtn)
            addView(enterBtn)
            addView(modeBtn)
        }
    }

    private fun createToolbarButton(text: String, onClick: () -> Unit): TextView {
        return TextView(this).apply {
            this.text = text
            textSize = 14f
            setTextColor(Color.parseColor("#666666"))
            setPadding(12.dpToPx(), 4.dpToPx(), 12.dpToPx(), 4.dpToPx())
            setBackgroundColor(Color.parseColor("#E0E0E0"))
            setOnClickListener { onClick() }
        }
    }

    private fun switchInputType() {
        switchToNextInputMethod(false)
    }

    override fun onDestroy() {
        super.onDestroy()
        if (engine != 0L) {
            nativeDestroyEngine(engine)
            engine = 0
        }
    }

    private fun Int.dpToPx(): Int {
        return (this * resources.displayMetrics.density).toInt()
    }

    data class Candidate(val text: String, val code: String, val weight: Double)
}