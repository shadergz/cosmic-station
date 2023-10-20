package emu.zenith.helpers

import android.content.Context
import android.util.Log
import emu.zenith.data.KernelModel
import emu.zenith.data.ZenithSettings
import java.io.File
import java.io.FileDescriptor
import java.io.FileInputStream

class KernelsHelper(val context: Context) {
    val kernelList = mutableListOf<KernelModel>()
    private val globalSettings = ZenithSettings.globalSettings

    private val kernelsDir: File = File(globalSettings.rootDirectory + "/Kernels")

    init {
        if (!kernelsDir.exists())
            kernelsDir.mkdirs()
        if (kernelsDir.exists() && kernelsDir.isDirectory) {
            kernelsDir.listFiles()?.forEach {
                add(it.path)
            }
        }
    }

    fun add(filePath: String) {
        val kernelFile = File(filePath)
        // Validating if we are working in the application's root directory
        val kernelName = kernelFile.absolutePath
        kernelFile.path.apply {
            assert(contains(kernelsDir.path))
        }
        val injection = runCatching {
            val kernelStream = FileInputStream(kernelFile)
            val model = kernelAdd(kernelStream.fd)
            if (model.dataCRC == 0u) {
                throw Exception("Kernel $kernelName not found in your storage or not accessible")
            }
            model.biosFilename = kernelFile.name
            model.fileAlive = kernelStream
            kernelList.add(model)
        }
        injection.exceptionOrNull()
    }

    fun remove(kModel: KernelModel) {
        assert(kernelList.contains(kModel))
        val removed = runCatching {
            val validModelInfo = arrayOf(kModel.id, kModel.dataCRC)
            if (!kernelRemove(validModelInfo)) {
                throw Exception("Unable to remove kernel with ID, CRC: $validModelInfo")
            }
        }
        if (removed.isSuccess)
            kernelList.remove(kModel)
    }

    fun setActive(kModel: KernelModel) {
        val kernelCRC = kModel.dataCRC
        assert(kernelList.contains(kModel))
        if (kModel.selected) {
            Log.d("Zenith", "Kernel ID, CRC $kernelCRC is already kSelected; this may be an issue")
            return
        }
        if (!kernelSet(kernelCRC))
            throw Exception("Can't set the kernel with CRC ($kernelCRC) as active")
        kernelList.find { it == kModel }?.apply {
            selected = true
        }
    }

    private external fun kernelAdd(descriptor: FileDescriptor): KernelModel
    private external fun kernelSet(kCRC: UInt): Boolean
    private external fun kernelRemove(kFDwCRC: Array<UInt>): Boolean
}