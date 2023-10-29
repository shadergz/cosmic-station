package emu.zenith.helpers

import androidx.lifecycle.ViewModel
import com.google.gson.Gson
import com.google.gson.JsonSyntaxException
import emu.zenith.data.DriverMeta
import emu.zenith.data.ZenithSettings
import java.io.File
import java.io.FileInputStream
import java.io.IOException
import java.util.zip.ZipEntry
import java.util.zip.ZipException
import java.util.zip.ZipInputStream

data class DriverContainer(
    val meta: DriverMeta,
    val drvPath: String,
    var selected: Boolean
)

class DriverHelperModel : ViewModel() {

    companion object {
        val driverList = mutableListOf<DriverContainer>()
        val settings = ZenithSettings.globalSettings

        private val driversDir = File(settings.appStorage, "Drivers")
        private val driversPack: Array<File> get() =
            driversDir.let {
                it.listFiles()!!
            }
        fun getVendorDriver() : DriverContainer {
            val info = DriverMeta("Vulkan", "Vendor driver", "Qualcomm", "Unknown", "Adreno", "Unknown", "31", "vulkan-adreno.so")
            return DriverContainer(info, "/system/vendor/", false)
        }
        fun getInUse(default: Int): Int {
            driversPack.forEachIndexed { index, drv ->
                val drvFiles = File(drv.path).listFiles()!!
                // The first index is always the system driver, so we need to increment the return value by one
                if (drvFiles.first { it.extension == "so" }.path == settings.customDriver)
                    return index + 1
            }
            return default
        }
        external fun switchTurboMode(enable: Boolean)
    }

    init {
        if (!driversDir.exists())
            driversDir.mkdirs()
    }
    private val jsonMarsh = Gson()

    private fun loadDriverDir(drvDir: String) {
        val metaNotMashed = File("$drvDir/meta.json").let {
            if (!it.exists())
                throw IOException("Unable to locate the metadata file for the specified driver")
            it.readText()
        }
        val threat = runCatching {
            val driver = jsonMarsh.fromJson(metaNotMashed, DriverMeta::class.java)
            val info = DriverContainer(driver, drvDir, false)
            
            assert(File("${info.drvPath}/${info.meta.libraryName}").exists())
            driverList.add(info)
        }
        if (threat.isFailure) {
            val cause = threat.exceptionOrNull()?.cause
            if (threat.exceptionOrNull() is JsonSyntaxException)
                throw RuntimeException("The 'meta.json' file contains invalid fields, $cause")
        }
    }

    fun activateDriver(info: DriverContainer) {
        val drvPath = "${info.drvPath}/${info.meta.libraryName}"
        settings.customDriver = drvPath
        driverList.forEach {
            it.selected = it.drvPath == info.drvPath
        }
    }
    
    private fun installDriver(packagePath: String) {
        val cherry = FileInputStream(packagePath).let {
            assert(File(packagePath).exists())
            ZipInputStream(it)
        }
        val theHeat = ByteArray(4096)
        val dirOutput = File(driversDir, File(packagePath).name)

        val extraction = runCatching {
            var drvEntry: ZipEntry? = cherry.nextEntry
            while (drvEntry != null) {
                val uniqueStream = File(dirOutput, drvEntry.name).let {
                    // We don't expect directories here
                    assert(it.isFile)
                    it.outputStream()
                }
                var uniqueLen: Int
                uniqueStream.use { stream ->
                    while (cherry.read(theHeat).also { uniqueLen = it } > 0)
                        stream.write(theHeat, 0, uniqueLen)
                }
                drvEntry = cherry.nextEntry
            }
        }.onFailure {
            if (it is ZipException)
                throw IOException("Some package entry $packagePath failed to be extracted, cause ${it.cause}")
        }
        extraction.exceptionOrNull()
        loadDriverDir(packagePath)
    }

    fun uninstallDriver(info: DriverContainer) {
        val drvDir = File(info.drvPath)
        if (info.drvPath.contains("/vendor/"))
            return
        if (drvDir.exists())
            drvDir.deleteRecursively()
    }
    
    fun getInstalledDrivers(): List<DriverContainer> {
        driversPack.forEach { driverDir ->
            val wasInstalled = driverList.filter {
                it.drvPath == driverDir.path
            }
            if (wasInstalled.isEmpty())
                loadDriverDir(driverDir.path)
        }
        return driverList
    }
}