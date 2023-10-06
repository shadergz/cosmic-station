package emu.zenith.prefers

import android.content.Intent
import android.os.Bundle
import android.provider.Settings
import androidx.appcompat.app.AppCompatActivity
import androidx.core.view.WindowCompat
import emu.zenith.R
import emu.zenith.data.Permissions
import emu.zenith.databinding.KernelActivityBinding
import emu.zenith.helper.KernelsHelper
import emu.zenith.helper.PermissionHelper

class KernelActivity : AppCompatActivity() {
    private val binding by lazy { KernelActivityBinding.inflate(layoutInflater) }

    private lateinit var checkStorage: PermissionHelper
    private val kernels by lazy { KernelsHelper(this) }

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
    }

    override fun onResume() {
        super.onResume()
        checkStorage.checkForPermission()
    }
}