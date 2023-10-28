package emu.zenith.data

data class DriverMeta(
    val name: String,
    val description: String,
    val author: String,
    val packageVersion: String,
    val vendor: String,
    val driverVersion: String,
    val minApi: String,
    val libraryName: String,
)
