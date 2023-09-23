package emu.zenith.prefers

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import emu.zenith.databinding.BiosActivityBinding

class BiosActivity : AppCompatActivity() {
    private val binding by lazy { BiosActivityBinding.inflate(layoutInflater) }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)
    }
}