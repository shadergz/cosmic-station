package emu.cosmic.data

import androidx.annotation.Keep
import java.io.FileInputStream

// For some reason, proguard-rules.pro can't keep these fields from KernelModel in release packages
@Keep
data class BiosInfo(
    var position: Int,

    var fileAlive: FileInputStream,
    var selected: Boolean,

    var biosPath: String,
    var biosName: String,
    var biosDetails: String
) {
     override fun equals(other: Any?): Boolean {
         val info = other as BiosInfo
         return (biosPath == info.biosPath) &&
                (biosName == info.biosName) &&
                (biosDetails == info.biosDetails)
    }
    override fun hashCode(): Int {
        var result = position
        result = 31 * result + fileAlive.hashCode()
        result = 31 * result + selected.hashCode()
        result = 31 * result + biosPath.hashCode()
        result = 31 * result + biosName.hashCode()
        result = 31 * result + biosDetails.hashCode()
        return result
    }
}