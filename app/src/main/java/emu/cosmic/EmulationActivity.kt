package emu.cosmic

import android.content.Intent
import android.content.pm.ActivityInfo
import android.os.Bundle
import android.view.Surface
import android.view.SurfaceHolder
import androidx.activity.OnBackPressedCallback
import androidx.activity.viewModels
import androidx.appcompat.app.AppCompatActivity
import emu.cosmic.databinding.EmulationActivityBinding
import emu.cosmic.models.EmulationModel

class EmulationActivity : AppCompatActivity(), SurfaceHolder.Callback {
    private var emuSurface: Surface? = null
    private val binding by lazy { EmulationActivityBinding.inflate(layoutInflater) }
    private val status by viewModels<EmulationModel>()

    private val emuThread = Thread {
        status.checkRunning(true)
        runEmulatorVm()
        goBackToMain()
    }
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        requestedOrientation = ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE
        setContentView(binding.root)

        onBackPressedDispatcher.addCallback(object: OnBackPressedCallback(true) {
            override fun handleOnBackPressed() {
                goBackToMain()
            }
        })
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