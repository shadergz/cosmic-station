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
import emu.cosmic.databinding.EmulationActivityBinding
import emu.cosmic.models.EmulationModel

class EmulationActivity : AppCompatActivity(), SurfaceHolder.Callback {
    private var emuSurface: Surface? = null
    private val binding by lazy { EmulationActivityBinding.inflate(layoutInflater) }
    private val status by viewModels<EmulationModel>()
    private var fps = 0
    private var unitsSpecs = arrayOf(0, 0, 0)

    private val emuThread = Thread {
        status.checkRunning(true)
        runEmulatorVm()
        goBackToMain()
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
            "EE: ${"%02d".format(unitsSpecs[0])}ms " +
            "VU: ${"%02d".format(unitsSpecs[1])}ms " +
            "GS: ${"%02d".format(unitsSpecs[2])}ms "
        if (!status.isRunning()) {
            emuThread.start()
        }
    }
    private fun goBackToMain() {
        if (!status.isRunning())
            return
        stopEmulatorVm()
        runOnUiThread {
            val main = Intent(applicationContext, MainActivity::class.java).setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP)
            startActivity(main)
        }
        status.checkRunning(false)
        if (emuThread != Thread.currentThread())
            emuThread.join()
    }
    private external fun swtSurfaceContext(surface: Surface? = null)
    private external fun runEmulatorVm()
    private external fun stopEmulatorVm()
    override fun surfaceCreated(holder: SurfaceHolder) {
        swtSurfaceContext(holder.surface)
        emuSurface = holder.surface
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {}
    override fun surfaceDestroyed(holder: SurfaceHolder) {
        emuSurface = null
    }
}