package emu.zenith

import android.content.pm.PackageManager
import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import androidx.core.view.WindowCompat
import emu.zenith.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityMainBinding.inflate(layoutInflater)
        setContentView(binding.root)

        WindowCompat.setDecorFitsSystemWindows(window, true)
        binding.homeToolbar.apply {
            title = resources.getString(R.string.app_name)
            subtitle = getAppVersion()
        }
    }

}
fun MainActivity.getAppVersion(): String {
    applicationContext.packageManager.let {
        val packageInfo = if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.TIRAMISU) {
            packageManager.getPackageInfo(packageName, PackageManager.PackageInfoFlags.of(0))
        } else {
            @Suppress("deprecation") packageManager.getPackageInfo(packageName, 0)
        }
        return packageInfo.versionName
    }
}