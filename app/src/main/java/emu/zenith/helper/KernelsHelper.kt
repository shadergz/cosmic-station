package emu.zenith.helper

import android.content.Context
import android.net.Uri
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
            if (model.kCRCId == 0u) {
                throw Exception("Kernel $kernelName not found in your storage or not accessible")
            }

            model.fileAlive = kernelStream
            kernelList.add(model)
        }
        injection.exceptionOrNull()
    }

    private external fun kernelAdd(descriptor: FileDescriptor): KernelModel
    external fun kernelSet(kCRCwFd: Array<Int>): KernelModel
    external fun kernelRemove(kCRCwFd: Array<Int>): Boolean
}