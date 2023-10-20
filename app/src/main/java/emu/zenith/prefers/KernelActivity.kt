package emu.zenith.prefers

import android.content.Intent
import android.os.Bundle
import android.provider.Settings
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.WindowCompat
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.viewbinding.ViewBinding
import emu.zenith.R
import emu.zenith.adapters.GenericListContainer
import emu.zenith.adapters.SelectableViewAdapter
import emu.zenith.data.Permissions
import emu.zenith.databinding.KernelActivityBinding
import emu.zenith.helpers.KernelsHelper
import emu.zenith.helpers.PermissionHelper
import emu.zenith.helpers.views.KernelViewItem

class KernelActivity : AppCompatActivity() {
    private val binding by lazy { KernelActivityBinding.inflate(layoutInflater) }

    private val adapter = SelectableViewAdapter(0)

    private lateinit var checkStorage: PermissionHelper
    private val kernels by lazy { KernelsHelper(this) }

    private fun feedAdapter() {
        val validKernels = mutableListOf<GenericListContainer<out ViewBinding>>()
        kernels.kernelList.forEachIndexed { _, kernel ->
            validKernels.add(KernelViewItem(this, kernel).apply {})
        }
        adapter.feedAdapter(validKernels)
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)
        WindowCompat.setDecorFitsSystemWindows(window, false)
        binding.appToolBar.headerToolBar.apply {
            title = resources.getString(R.string.toolbar_kernel)
        }
        setSupportActionBar(binding.appToolBar.headerToolBar)
        supportActionBar?.setDisplayHomeAsUpEnabled(true)

        // It needs to be created here because the Context may not be valid
        checkStorage = PermissionHelper(this, Permissions.storageAccess) {
            val launcher = Intent(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION)
            startActivity(launcher)
        }

        val manager = LinearLayoutManager(this)
        binding.kernelRecycler.adapter = adapter
        binding.kernelRecycler.layoutManager = manager

        feedAdapter()
    }

    override fun onResume() {
        super.onResume()
        checkStorage.checkForPermission()
    }
}