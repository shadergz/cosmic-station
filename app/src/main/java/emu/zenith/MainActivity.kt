package emu.zenith

import android.content.Intent
import android.content.pm.PackageManager
import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.provider.Settings
import android.widget.Toast
import androidx.core.view.WindowCompat
import com.google.android.material.appbar.MaterialToolbar
import emu.zenith.data.Permissions
import emu.zenith.data.ZenithSettings
import emu.zenith.databinding.MainActivityBinding
import emu.zenith.dialogs.AboutDialog
import emu.zenith.helpers.PermissionHelper
import java.time.LocalDateTime
import java.time.format.DateTimeFormatter

class MainActivity : AppCompatActivity() {
    private val binding by lazy {
        MainActivityBinding.inflate(layoutInflater)
    }
    private lateinit var toolBar: MaterialToolbar
    private lateinit var checkStorage: PermissionHelper

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)

        WindowCompat.setDecorFitsSystemWindows(window, true)
        toolBar = binding.appToolBar

        toolBar.apply {
            title = resources.getString(R.string.app_name)
            subtitle = getAppVersion()
        }

        setMenuItemHandler()
        // It needs to be created here because the Context may not be valid
        checkStorage = PermissionHelper(this, Permissions.storageAccess) {
            val launcher = Intent(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION)
            startActivity(launcher)
        }
    }

    private fun setMenuItemHandler() {
        toolBar.apply {
            setOnMenuItemClickListener {
                when (it.itemId) {
                    R.id.aboutMainItem -> {
                        AboutDialog().show(supportFragmentManager, AboutDialog.DialogTag)
                    }
                    else -> changeActivity(it.itemId)
                }
                true
            }
        }
    }
    private fun changeActivity(itemId: Int) {
        val toActivity: Any = when (itemId) {
            R.id.settingsMainItem -> SettingsActivity::class.java
            else -> {}
        }
        val respIntent = Intent(this, toActivity as Class<*>)
        respIntent.apply {
            startActivity(this)
        }
    }

    override fun onResume() {
        super.onResume()
        checkStorage.checkForPermission()

        if (!ZenithSettings.updateSettings)
            return
        val dt = LocalDateTime.now()
        DateTimeFormatter.ofPattern("MM/dd/yyyy HH:mm:ss").let {
            val time = dt.format(it)
            syncStateValues("Time: $time")

            ZenithSettings.updateSettings = false
            Toast.makeText(this, "Updated settings", Toast.LENGTH_SHORT).show()
        }
    }
    private external fun syncStateValues(dateTime: String)
}
fun MainActivity.getAppVersion(): String {
    applicationContext.packageManager.let {
        val packageInfo = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            packageManager.getPackageInfo(packageName, PackageManager.PackageInfoFlags.of(0))
        } else {
            packageManager.getPackageInfo(packageName, 0)
        }
        return packageInfo.versionName
    }
}