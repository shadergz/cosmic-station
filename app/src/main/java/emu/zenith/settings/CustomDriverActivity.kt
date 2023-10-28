package emu.zenith.settings

import android.os.Bundle
import androidx.activity.viewModels
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.WindowCompat
import androidx.recyclerview.widget.LinearLayoutManager
import com.google.android.material.radiobutton.MaterialRadioButton
import emu.zenith.R
import emu.zenith.adapters.SelectableViewAdapter
import emu.zenith.data.ZenithSettings
import emu.zenith.databinding.DriverActivityBinding
import emu.zenith.helpers.DriverHelperModel
import emu.zenith.views.DriverViewItem

class CustomDriverActivity : AppCompatActivity() {
    val binding by lazy { DriverActivityBinding.inflate(layoutInflater) }

    private val driverModel: DriverHelperModel by viewModels()
    private val adapter = SelectableViewAdapter(DriverHelperModel.getInUse(0))
    private val settings = ZenithSettings.globalSettings

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)

        binding.appToolBar.apply {
            title = resources.getString(R.string.toolbar_global_drivers)
        }

        val manager = LinearLayoutManager(this)
        binding.drvRecycler.adapter = adapter
        binding.drvRecycler.layoutManager = manager

        WindowCompat.setDecorFitsSystemWindows(window, false)
        setSupportActionBar(binding.appToolBar)
        supportActionBar?.setDisplayHomeAsUpEnabled(true)

        DriverHelperModel.driverList.let { list ->
            val driver = DriverHelperModel.getVendorDriver()
            if (list.none { it.drvPath == driver.drvPath })
                list.add(driver)
        }

        driverModel.getInstalledDrivers().forEachIndexed { index, drv ->
            adapter.insertItem(index, DriverViewItem(this, drv).apply {
                onClick = {
                    if (it is MaterialRadioButton)
                        it.isChecked = true
                    driverModel.activateDriver(drv)
                    adapter.selectItem(index)
                }
                onDelete = { _, _ ->
                    driverModel.uninstallDriver(drv)
                }

                if (index == adapter.selectedPos) {
                    driverModel.activateDriver(drv)
                    adapter.selectItem(index)
                }
            })
        }

    }
}