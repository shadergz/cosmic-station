package emu.zenith.helper

import android.content.Context
import emu.zenith.data.KernelModel
import emu.zenith.data.ZenithSettings
import java.io.File
import java.io.FileDescriptor

class KernelsHelper(val context: Context) {
    private val kernelList = mutableListOf<KernelModel>()
    private val globalSettings = ZenithSettings.globalSettings

    init {
        val kernelsDir = File(globalSettings.rootDirectory + "/Kernels")
        assert(kernelList.size == 0)

        if (!kernelsDir.exists())
            kernelsDir.mkdirs()
    }

    external fun kernelAdd(descriptor: FileDescriptor): KernelModel
    external fun kernelSelect(kCRCwFd: Array<Int>): KernelModel
    external fun kernelRemove(kCRCwFd: Array<Int>): Boolean
}