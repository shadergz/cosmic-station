package emu.zenith.helpers

import android.content.Context
import emu.zenith.data.KernelModel
import emu.zenith.data.ZenithSettings
import java.io.File
import java.io.FileDescriptor
import java.io.FileInputStream
import java.io.IOException

class KernelsHelper(val context: Context) {
    companion object {
        private val kernelsList = mutableListOf<KernelModel>()

        fun toDefault() {
            kernelsList.clear()
            discardAllKernels()
        }

        private external fun addKernel(descriptor: FileDescriptor, position: Int): KernelModel
        private external fun setKernel(position: Int): Int
        private external fun removeKernel(posFd: IntArray): Boolean
        private external fun discardAllKernels()
        external fun getRunningKernel(defaultPos: Int): Int
    }
    private val globalSettings = ZenithSettings.globalSettings
    private val kernelsDir: File = File(globalSettings.appStorage + "/Kernels")
    init {
        if (!kernelsDir.exists())
            kernelsDir.mkdirs()
        assert(kernelsDir.exists() && kernelsDir.isDirectory)
    }

    fun getKernels() : List<KernelModel> {
        var position = 0
        kernelsDir.listFiles()?.forEach { biosFile ->
            runCatching {
                val resident = kernelsList.first {
                    it.biosFilename == biosFile.name
                }
                resident
            }.onFailure {
                if (it is NoSuchElementException || it is NullPointerException)
                    loadKernelModel(biosFile.path, position++)
            }
        }
        return kernelsList
    }
    private fun loadKernelModel(filePath: String, position: Int) {
        val kernelFile = File(filePath)
        // Validating if we are working in the application's root directory
        val kernelName = kernelFile.absolutePath
        kernelFile.path.apply {
            assert(contains(kernelsDir.path))
        }
        val injection = runCatching {
            val kernelStream = FileInputStream(kernelFile)
            val model = addKernel(kernelStream.fd, position)
            if (model.biosName.isEmpty()) {
                throw IOException("Kernel $kernelName not found in your storage or not accessible")
            }
            model.biosFilename = kernelFile.name
            model.fileAlive = kernelStream
            model
        }
        if (injection.isSuccess)
            kernelsList.add(injection.getOrThrow())
    }

    fun unloadKernel(position: Int) {
        val model = kernelsList[position]
        val removed = runCatching {
            val validModelInfo = intArrayOf(0, model.position)
            if (!removeKernel(validModelInfo)) {
                throw Exception("Unable to remove kernel with fd, pos: $validModelInfo")
            }
        }
        if (removed.isSuccess)
            kernelsList.remove(model)
    }

    fun activateKernel(position: Int) : Int {
        val previous = setKernel(position)
        if (previous != position) {
            for (kernel in kernelsList) {
                if (kernel.position == previous) {
                    assert(kernel.selected)
                    kernelsList[previous].selected = false
                }
            }
        }
        kernelsList[position].selected = true
        return previous
    }
}