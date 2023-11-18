package emu.cosmic.settings

import android.net.Uri
import android.os.Bundle
import androidx.activity.viewModels
import androidx.appcompat.app.AppCompatActivity
import androidx.core.net.toUri
import androidx.core.view.WindowCompat
import androidx.recyclerview.widget.LinearLayoutManager
import com.google.android.material.radiobutton.MaterialRadioButton
import emu.cosmic.R
import emu.cosmic.adapters.SelectableViewAdapter
import emu.cosmic.data.CosmicSettings
import emu.cosmic.databinding.DriverActivityBinding
import emu.cosmic.helpers.DriverHelperModel
import emu.cosmic.listeners.pathSolver
import emu.cosmic.views.DriverViewItem
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.runBlocking
import kotlinx.coroutines.withContext
import java.io.File

class CustomDriverActivity : AppCompatActivity() {
    val binding by lazy { DriverActivityBinding.inflate(layoutInflater) }

    private val driverModel: DriverHelperModel by viewModels()
    private val adapter = SelectableViewAdapter(DriverHelperModel.getInUse(0))
    private val settings = CosmicSettings.globalSettings

    val extract = registerForActivityResult(OpenASFContract()) { result: Uri? ->
        val pack = File(result?.path!!)
        runBlocking {
            withContext(Dispatchers.IO) {
                val name = pathSolver(pack.toUri())
                contentResolver.openInputStream(result).use {
                    driverModel.installDriver(name, it!!)
                }
            }
            restartActivity()
        }
    }

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
            if (list.none { it.driverPath == driver.driverPath })
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
        binding.installDriver.setOnClickListener {
            extract.launch("application/zip")
        }

    }
}