package emu.zenith.helpers

import android.content.Context
import emu.zenith.data.BiosModel
import emu.zenith.data.ZenithSettings
import java.io.File
import java.io.FileDescriptor
import java.io.FileInputStream
import java.io.IOException

class BiosHelper(val context: Context) {
    companion object {
        private val biosList = mutableListOf<BiosModel>()

        fun toDefault() {
            biosList.clear()
            cleanAllBios()
        }

        private external fun addBios(descriptor: FileDescriptor, position: Int): BiosModel
        private external fun setBios(position: Int): Int
        private external fun removeBios(posFd: IntArray): Boolean
        private external fun cleanAllBios()
        external fun getRunningBios(defaultPos: Int): Int
    }
    private val globalSettings = ZenithSettings.globalSettings
    private val biosDir = File(globalSettings.appStorage + "/System")
    init {
        if (!biosDir.exists())
            biosDir.mkdirs()
        assert(biosDir.exists() && biosDir.isDirectory)
    }

    fun getAllInstalled() : List<BiosModel> {
        var position = 0
        biosDir.listFiles()?.forEach { biosFile ->
            runCatching {
                val resident = biosList.first {
                    it.biosFilename == biosFile.name
                }
                resident
            }.onFailure {
                if (it is NoSuchElementException || it is NullPointerException)
                    loadBios(biosFile.path, position++)
            }
        }
        return biosList
    }
    private fun loadBios(filePath: String, position: Int) {
        val biosf = File(filePath)
        // Validating if we are working in the application's root directory
        val biosName = biosf.absolutePath
        biosf.path.apply {
            assert(contains(biosDir.path))
        }
        val injection = runCatching {
            val biosStream = FileInputStream(biosf)
            val model = addBios(biosStream.fd, position)
            if (model.biosName.isEmpty()) {
                throw IOException("BIOS $biosName not found in your storage or not accessible")
            }
            model.biosFilename = biosf.name
            model.fileAlive = biosStream
            model
        }
        if (injection.isSuccess)
            biosList.add(injection.getOrThrow())
    }

    fun unloadBios(position: Int) {
        val model = biosList[position]
        val removed = runCatching {
            val validModelInfo = intArrayOf(0, model.position)
            if (!removeBios(validModelInfo)) {
                throw Exception("Unable to remove BIOS with fd, pos: $validModelInfo")
            }
        }
        if (removed.isSuccess)
            biosList.remove(model)
    }

    fun activateBios(position: Int) : Int {
        val previous = setBios(position)
        if (previous != position) {
            for (kernel in biosList) {
                if (kernel.position == previous) {
                    assert(kernel.selected)
                    biosList[previous].selected = false
                }
            }
        }
        biosList[position].selected = true
        return previous
    }
}