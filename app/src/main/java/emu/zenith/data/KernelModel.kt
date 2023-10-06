package emu.zenith.data

import java.io.FileInputStream

data class KernelModel(
    var kCRCId: UInt,
    var fileAlive: FileInputStream,
    var selected: Boolean,

    var kObject: String,
    var originVersion: String,
    var kName: String
)

