package emu.zenith.settings

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import emu.zenith.R
import emu.zenith.databinding.DriverActivityBinding

class CustomDriverActivity : AppCompatActivity() {
    val binding by lazy { DriverActivityBinding.inflate(layoutInflater) }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)

        binding.appToolBar.apply {
            title = resources.getString(R.string.toolbar_global_drivers)
        }
    }
}