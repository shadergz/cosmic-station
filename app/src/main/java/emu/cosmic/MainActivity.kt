package emu.cosmic

import android.content.Intent
import android.content.pm.PackageManager
import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.provider.Settings
import android.widget.Toast
import androidx.core.view.WindowCompat
import androidx.fragment.app.Fragment
import androidx.fragment.app.FragmentActivity
import androidx.viewpager2.adapter.FragmentStateAdapter
import androidx.viewpager2.widget.ViewPager2
import com.google.android.material.appbar.MaterialToolbar
import com.google.android.material.tabs.TabLayout
import emu.cosmic.data.Permissions
import emu.cosmic.data.CosmicSettings
import emu.cosmic.databinding.MainActivityBinding
import emu.cosmic.dialogs.AboutDialog
import emu.cosmic.fragments.GamesFolderFragment
import emu.cosmic.helpers.PermissionHelper
import java.time.LocalDateTime
import java.time.format.DateTimeFormatter

class MainActivity : AppCompatActivity() {
    private val binding by lazy {
        MainActivityBinding.inflate(layoutInflater)
    }
    private lateinit var toolBar: MaterialToolbar
    private lateinit var checkStorage: PermissionHelper

    private inner class PagerAdapter(fragment: FragmentActivity) : FragmentStateAdapter(fragment) {
        override fun getItemCount(): Int {
            return 3
        }
        override fun createFragment(position: Int): Fragment {
            return when (position) {
                0 -> GamesFolderFragment()
                else -> GamesFolderFragment()
            }
        }
    }
    private lateinit var tabAdapter: PagerAdapter

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(binding.root)

        WindowCompat.setDecorFitsSystemWindows(window, true)
        toolBar = binding.appToolBar

        toolBar.apply {
            title = "${resources.getString(R.string.app_name)} Alpha"
            subtitle = getAppVersion()
        }
        tabAdapter = PagerAdapter(this)
        binding.mainPager.adapter = tabAdapter

        setMenuItemHandler()
        setTabLayoutItems()
        // It needs to be created here because the Context may not be valid
        checkStorage = PermissionHelper(this, Permissions.storageAccess) {
            val launcher = Intent(Settings.ACTION_MANAGE_ALL_FILES_ACCESS_PERMISSION)
            startActivity(launcher)
        }
    }
    private fun setTabLayoutItems() {
        binding.mainTabLayout.addOnTabSelectedListener(object: TabLayout.OnTabSelectedListener {
            override fun onTabSelected(tab: TabLayout.Tab?) {
                binding.mainPager.currentItem = tab?.position!!
            }
            override fun onTabUnselected(tab: TabLayout.Tab?) {}
            override fun onTabReselected(tab: TabLayout.Tab?) {}
        })
        binding.mainPager.registerOnPageChangeCallback(object: ViewPager2.OnPageChangeCallback() {
            override fun onPageSelected(position: Int) {
                super.onPageSelected(position)
                binding.mainTabLayout.getTabAt(position)?.select()
            }
        })
    }

    private fun setMenuItemHandler() {
        toolBar.apply {
            setOnMenuItemClickListener {
                when (it.itemId) {
                    R.id.aboutMainItem -> AboutDialog().show(supportFragmentManager, AboutDialog.DialogTag)
                    else -> changeActivity(it.itemId)
                }
                true
            }
        }
    }
    private fun changeActivity(itemId: Int) {
        val toActivity: Any = when (itemId) {
            R.id.settingsMainItem -> SettingsActivity::class.java
            R.id.bootBiosMainItem -> EmulationActivity::class.java
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

        if (!CosmicSettings.updateSettings)
            return
        val dt = LocalDateTime.now()
        DateTimeFormatter.ofPattern("MM/dd/yyyy HH:mm:ss").let {
            val time = dt.format(it)
            syncStateValues("Time: $time")

            CosmicSettings.updateSettings = false
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