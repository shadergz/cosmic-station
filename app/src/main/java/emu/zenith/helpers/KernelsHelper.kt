package emu.zenith.helpers

import android.content.Context
import android.util.Log
import emu.zenith.data.KernelModel
import emu.zenith.data.ZenithSettings
import java.io.File
import java.io.FileDescriptor
import java.io.FileInputStream

class KernelsHelper(val context: Context) {
    companion object {
        private val kernelsList = mutableListOf<KernelModel>()
    }
    private val globalSettings = ZenithSettings.globalSettings

    private val kernelsDir: File = File(globalSettings.rootDirectory + "/Kernels")
    init {
        if (!kernelsDir.exists())
            kernelsDir.mkdirs()
    }

    fun getKernels() : List<KernelModel> {
        if (kernelsDir.exists() && kernelsDir.isDirectory) {
            kernelsDir.listFiles()?.forEach { biosFile ->
                runCatching {
                    val resident = kernelsList.first {
                        it.biosFilename == biosFile.name
                    }
                    resident
                }.onFailure {
                    if (it is NoSuchElementException || it is NullPointerException)
                        loadKernelModel(biosFile.path)
                }
            }
        }
        return kernelsList
    }
    private fun loadKernelModel(filePath: String) {
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
            model
        }
        if (injection.isSuccess)
            kernelsList.add(injection.getOrNull()!!)
    }

    fun removeKernel(kModel: KernelModel) {
        assert(kernelsList.contains(kModel))
        val removed = runCatching {
            val validModelInfo = arrayOf(kModel.id, kModel.dataCRC)
            if (!kernelRemove(validModelInfo)) {
                throw Exception("Unable to remove kernel with ID, CRC: $validModelInfo")
            }
        }
        if (removed.isSuccess)
            kernelsList.remove(kModel)
    }

    fun activateKernel(kModel: KernelModel) {
        val kernelCRC = kModel.dataCRC
        assert(kernelsList.contains(kModel))
        if (kModel.selected) {
            Log.d("Zenith", "Kernel ID, CRC $kernelCRC is already selected; this may be an issue")
            return
        }
        if (!kernelSet(kernelCRC))
            throw Exception("Can't set the kernel with CRC ($kernelCRC) as active")
        kernelsList.find { it == kModel }?.apply {
            selected = true
        }
    }

    private external fun kernelAdd(descriptor: FileDescriptor): KernelModel
    private external fun kernelSet(crc: UInt): Boolean
    private external fun kernelRemove(crcFD: Array<UInt>): Boolean
}