package emu.zenith.settings

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.WindowCompat
import androidx.recyclerview.widget.LinearLayoutManager
import com.google.android.material.radiobutton.MaterialRadioButton
import emu.zenith.R
import emu.zenith.adapters.SelectableViewAdapter
import emu.zenith.databinding.BiosActivityBinding
import emu.zenith.helpers.BiosHelper
import emu.zenith.views.BiosViewItem

class BiosActivity : AppCompatActivity() {
    private val binding by lazy {
        BiosActivityBinding.inflate(layoutInflater)
    }

    private val biosHelper by lazy { BiosHelper(this) }
    private val adapter = SelectableViewAdapter(BiosHelper.getRunningBios(0))

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

        biosHelper.getAllInstalled().forEachIndexed { index, kernel ->
            adapter.insertItem(index, BiosViewItem(this@BiosActivity, kernel).apply {
                onClick = {
                    biosHelper.activateBios(index)
                    adapter.selectItem(index)
                    if (it is MaterialRadioButton && kernel.selected)
                        it.isChecked = true
                }
                onDelete = { _, _ ->
                    biosHelper.unloadBios(index)
                }

                if (index == adapter.selectedPos) {
                    biosHelper.activateBios(index)
                    adapter.selectItem(index)
                }
            })
        }
    }
}