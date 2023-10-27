package emu.zenith.helpers

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

data class DriverInfo(
    val driverMeta: DriverMeta,
    val drvPath: String,
    val vulkanPath: String
)

class DriverHelper {

    companion object {
        private val driverList = mutableListOf<DriverInfo>()

        external fun switchTurboMode(enable: Boolean)
    }

    private val settings = ZenithSettings.globalSettings
    private val driversDir = File(settings.appStorage + "/Drivers")

    init {
        if (!driversDir.exists())
            driversDir.mkdirs()
    }

    private fun loadDriverDir(drvDir: String) {
        val metaNotMashed = File("$drvDir/meta.json").let {
            if (!it.exists())
                throw IOException("Unable to locate the metadata file for the specified driver")
            it.readText()
        }
        val threat = runCatching {
            val driver = Gson().fromJson(metaNotMashed, DriverMeta::class.java)
            val info = DriverInfo(driver, drvDir, "$drvDir/${driver.libraryName}")
            
            assert(File(info.vulkanPath).exists())
            driverList.add(info)
        }
        if (threat.isFailure) {
            val cause = threat.exceptionOrNull()?.cause
            if (threat.exceptionOrNull() is JsonSyntaxException)
                throw RuntimeException("The 'meta.json' file contains invalid fields, $cause")
        }
    }
    
    private fun installDriver(packagePath: String) {
        val cherry = FileInputStream(packagePath).let {
            assert(File(packagePath).exists())
            ZipInputStream(it)
        }
        val theHeat = ByteArray(4096)

        val extraction = runCatching {
            var drvEntry: ZipEntry? = cherry.nextEntry
            while (drvEntry != null) {
                val uniqueStream = File(driversDir, drvEntry.name).let {
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
    }

    private fun uninstallDriver(info: DriverInfo) {
        val drvDir = File(info.drvPath)
        if (drvDir.exists())
            drvDir.deleteRecursively()
    }
    
    fun getInstalledDrivers(): List<DriverInfo> {
        driversDir.listFiles()?.forEach { driverDir ->
            val wasInstalled = driverList.filter {
                it.drvPath == driverDir.path
            }
            if (wasInstalled.isEmpty())
                loadDriverDir(driverDir.path)
        }
        return driverList
    }
}