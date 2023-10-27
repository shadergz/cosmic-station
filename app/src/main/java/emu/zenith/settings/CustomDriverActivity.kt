package emu.zenith.settings

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.WindowCompat
import androidx.recyclerview.widget.LinearLayoutManager
import emu.zenith.R
import emu.zenith.adapters.SelectableViewAdapter
import emu.zenith.databinding.DriverActivityBinding
import emu.zenith.helpers.DriverHelper
import emu.zenith.views.DriverViewItem

class CustomDriverActivity : AppCompatActivity() {
    val binding by lazy { DriverActivityBinding.inflate(layoutInflater) }

    private val drivers by lazy { DriverHelper() }
    private val holder = SelectableViewAdapter(0)

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)

        binding.appToolBar.apply {
            title = resources.getString(R.string.toolbar_global_drivers)
        }

        val manager = LinearLayoutManager(this)
        binding.drvRecycler.adapter = holder
        binding.drvRecycler.layoutManager = manager

        WindowCompat.setDecorFitsSystemWindows(window, false)
        setSupportActionBar(binding.appToolBar)
        supportActionBar?.setDisplayHomeAsUpEnabled(true)

        drivers.getInstalledDrivers().forEachIndexed { index, drv ->
            holder.insertItem(index, DriverViewItem(this, drv.driverMeta).apply {
                onClick = {}
            })
        }

    }
}