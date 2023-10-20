package emu.zenith.data

import androidx.annotation.Keep
import java.io.FileInputStream

// For some reason, proguard-rules.pro can't keep these fields from KernelModel in release packages
@Keep
data class KernelModel(
    var id: UInt,
    var dataCRC: UInt,

    var fileAlive: FileInputStream,
    var selected: Boolean,

    var biosFilename: String? = null,
    var biosName: String? = null,
    var biosDetails: String? = null
)

