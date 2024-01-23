package emu.cosmic

import android.annotation.SuppressLint
import android.content.Intent
import android.content.pm.ActivityInfo
import android.os.Bundle
import android.view.Surface
import android.view.SurfaceHolder
import android.view.WindowInsets
import androidx.activity.OnBackPressedCallback
import androidx.activity.viewModels
import androidx.appcompat.app.AppCompatActivity
import com.google.android.material.dialog.MaterialAlertDialogBuilder
import emu.cosmic.databinding.EmulationActivityBinding
import emu.cosmic.models.EmulationModel
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.runBlocking
import kotlinx.coroutines.sync.Semaphore
import kotlinx.coroutines.withContext

class EmulationActivity : AppCompatActivity(), SurfaceHolder.Callback {
    private var emuSurface: Surface? = null
    private val binding by lazy { EmulationActivityBinding.inflate(layoutInflater) }
    private val status by viewModels<EmulationModel>()
    private var fps = 0
    private var unitsSpecs = arrayOf(.0, .0, .0)

    private val emuThread = Thread {
        status.checkRunning(true)
        startEmulator()
        goBackToMain()
    }
    init {
        activity = this
    }

    companion object {
        lateinit var activity: EmulationActivity

        @Suppress("unused") @JvmStatic
        fun displayAlert(title: String, msg: String) {
            val quit = Semaphore(1, 1)
            activity.stopEmulator()

            activity.runOnUiThread {
                MaterialAlertDialogBuilder(activity)
                    .setTitle(title)
                    .setMessage(msg)
                    .setCancelable(false)
                    .setPositiveButton(activity.getString(R.string.positive)) { _, _ ->
                        quit.release()
                }.show()
            }
            runBlocking {
                withContext(Dispatchers.Default) {
                    quit.acquire()
                    activity.fps = 0
                    activity.goBackToMain()
                }
            }

        }
    }

    @SuppressLint("SetTextI18n")
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        requestedOrientation = ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE
        setContentView(binding.root)
        binding.emulationSurface.holder.addCallback(this)
        window.setDecorFitsSystemWindows(false)
        window.decorView.windowInsetsController?.apply {
            hide(WindowInsets.Type.statusBars())
        }

        onBackPressedDispatcher.addCallback(object: OnBackPressedCallback(true) {
            override fun handleOnBackPressed() {
                goBackToMain()
            }
        })
        binding.perfFps.text = "$fps FPS\n" +
            "EE: ${"%02.4f".format(unitsSpecs[0])}ms " +
            "VU: ${"%02.4f".format(unitsSpecs[1])}ms " +
            "GS: ${"%02.4f".format(unitsSpecs[2])}ms"
        if (!status.isRunning()) {
            emuThread.start()
        }
    }
    private fun goBackToMain() {
        if (!status.isRunning())
            return
        stopEmulator()
        runOnUiThread {
            val main = Intent(applicationContext, MainActivity::class.java).setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP)
            startActivity(main)
        }
        status.checkRunning(false)
        if (emuThread != Thread.currentThread())
            emuThread.join()
    }
    private external fun swtSurfaceContext(surface: Surface? = null)
    private external fun startEmulator()
    private external fun stopEmulator()
    override fun surfaceCreated(holder: SurfaceHolder) {
        swtSurfaceContext(holder.surface)
        emuSurface = holder.surface
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {}
    override fun surfaceDestroyed(holder: SurfaceHolder) {
        emuSurface = null
    }
}