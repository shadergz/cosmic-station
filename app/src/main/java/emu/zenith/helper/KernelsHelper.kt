package emu.zenith.helper

import android.content.Context
import android.net.Uri
import android.util.Log
import emu.zenith.data.KernelModel
import emu.zenith.data.ZenithSettings
import java.io.File
import java.io.FileDescriptor
import java.io.FileInputStream

class KernelsHelper(val context: Context) {
    private val kernelList = mutableListOf<KernelModel>()
    private val globalSettings = ZenithSettings.globalSettings

    private val kernelsDir: File = File(globalSettings.rootDirectory + "/Kernels")

    init {
        if (!kernelsDir.exists())
            kernelsDir.mkdirs()
    }

    fun add(kernelUri: Uri) {
        val kernelFile = File(kernelUri.path!!)
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
        val info = arrayOf(kModel.id, kModel.dataCRC)
        assert(kernelList.contains(kModel))
        if (kModel.selected) {
            Log.d("Zenith", "Kernel ID, CRC $info is already selected; this may be an issue")
            return
        }
        if (!kernelSet(info))
            throw Exception("Can't set the kernel with ID, CRC ($info) as active")
        kernelList.find { it == kModel }?.apply {
            selected = true
        }
    }

    private external fun kernelAdd(descriptor: FileDescriptor): KernelModel
    private external fun kernelSet(kCRCwFd: Array<UInt>): Boolean
    private external fun kernelRemove(kCRCwFd: Array<UInt>): Boolean
}