package emu.zenith.data

data class KernelModel(
    val kCRCId: Int,
    val kFD: Int,
    var selected: Boolean,

    val kObject: String,
    val originVersion: String,
    val kName: String
)

