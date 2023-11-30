package emu.cosmic

import android.content.Intent
import android.content.pm.ActivityInfo
import android.os.Bundle
import android.view.Surface
import android.view.SurfaceHolder
import androidx.activity.OnBackPressedCallback
import androidx.appcompat.app.AppCompatActivity
import emu.cosmic.databinding.EmulationActivityBinding

class EmulationActivity : AppCompatActivity(), SurfaceHolder.Callback {
    private var emuSurface: Surface? = null
    private val binding by lazy { EmulationActivityBinding.inflate(layoutInflater) }
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        requestedOrientation = ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE
        setContentView(binding.root)
        onBackPressedDispatcher.addCallback(object: OnBackPressedCallback(true) {
            override fun handleOnBackPressed() {
                val main = Intent(applicationContext, MainActivity::class.java).setFlags(Intent.FLAG_ACTIVITY_CLEAR_TOP)
                startActivity(main)
            }
        })
        runEmuVm()
    }
    private external fun swtSurfaceContext(surface: Surface? = null)
    private external fun runEmuVm()
    override fun surfaceCreated(holder: SurfaceHolder) {
        swtSurfaceContext(holder.surface)
        emuSurface = holder.surface
    }

    override fun surfaceChanged(holder: SurfaceHolder, format: Int, width: Int, height: Int) {}
    override fun surfaceDestroyed(holder: SurfaceHolder) {
        emuSurface = null
    }
}