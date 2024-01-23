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
import emu.cosmic.databinding.BiosActivityBinding
import emu.cosmic.helpers.BiosHelper
import emu.cosmic.listeners.pathSolver
import emu.cosmic.views.BiosViewItem
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.runBlocking
import kotlinx.coroutines.withContext
import java.io.File

class BiosActivity : AppCompatActivity() {
    private val binding by lazy {
        BiosActivityBinding.inflate(layoutInflater)
    }

    private val biosModel: BiosHelper by viewModels()
    private val adapter = SelectableViewAdapter(BiosHelper.getInUse(0))

    val install = registerForActivityResult(OpenASFContract()) { result: Uri? ->
        if (result == null)
            return@registerForActivityResult

        val bios = File(result.path!!)
        val storageFile = contentResolver.openInputStream(result)
        val destNameExt = pathSolver(bios.toUri(), "").substringAfterLast("/")
        val installationDir = File("${BiosHelper.biosDir}/$destNameExt")

        runBlocking {
            withContext(Dispatchers.IO) {
                installationDir.createNewFile()
            }
            val systemBios = installationDir.outputStream()
            storageFile?.copyTo(systemBios)
            restartActivity()
        }
    }

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
        binding.addBios.setOnClickListener {
            install.launch("*/*")
        }
    }
}

fun AppCompatActivity.restartActivity() {
    val savedContext = intent
    finish()
    startActivity(savedContext)
}
