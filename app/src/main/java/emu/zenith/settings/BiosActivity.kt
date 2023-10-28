package emu.zenith.settings

import android.os.Bundle
import androidx.activity.viewModels
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.WindowCompat
import androidx.recyclerview.widget.LinearLayoutManager
import com.google.android.material.radiobutton.MaterialRadioButton
import emu.zenith.R
import emu.zenith.adapters.SelectableViewAdapter
import emu.zenith.databinding.BiosActivityBinding
import emu.zenith.helpers.BiosHelperModel
import emu.zenith.views.BiosViewItem

class BiosActivity : AppCompatActivity() {
    private val binding by lazy {
        BiosActivityBinding.inflate(layoutInflater)
    }

    private val biosModel: BiosHelperModel by viewModels()
    private val adapter = SelectableViewAdapter(BiosHelperModel.getRunningBios(0))

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)
        WindowCompat.setDecorFitsSystemWindows(window, false)
        binding.appToolBar.apply {
            title = resources.getString(R.string.toolbar_global_bios)
        }
        setSupportActionBar(binding.appToolBar)
        supportActionBar?.setDisplayHomeAsUpEnabled(true)

        val manager = LinearLayoutManager(this)
        binding.biosRecycler.adapter = adapter
        binding.biosRecycler.layoutManager = manager

        biosModel.getAllInstalled().forEachIndexed { index, bios ->
            adapter.insertItem(index, BiosViewItem(this@BiosActivity, bios).apply {
                onClick = {
                    if (it is MaterialRadioButton)
                        it.isChecked = true
                    biosModel.activateBios(index)
                    adapter.selectItem(index)
                }
                onDelete = { _, _ ->
                    biosModel.unloadBios(index)
                }

                if (index == adapter.selectedPos) {
                    biosModel.activateBios(index)
                    adapter.selectItem(index)
                }
            })
        }
    }
}