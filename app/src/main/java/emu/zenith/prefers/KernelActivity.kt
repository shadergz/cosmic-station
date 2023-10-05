package emu.zenith.prefers

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.WindowCompat
import emu.zenith.R
import emu.zenith.databinding.KernelActivityBinding

class KernelActivity : AppCompatActivity() {
    private val binding by lazy { KernelActivityBinding.inflate(layoutInflater) }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)
        WindowCompat.setDecorFitsSystemWindows(window, false)
        binding.appToolBar.headerToolBar.apply {
            title = resources.getString(R.string.toolbar_kernel)
        }
        setSupportActionBar(binding.appToolBar.headerToolBar)
        supportActionBar?.setDisplayHomeAsUpEnabled(true)
    }
}