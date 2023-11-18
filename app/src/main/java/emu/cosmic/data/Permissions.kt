package emu.cosmic.data

import android.Manifest

class Permissions(val permissions: Array<String>) {
    companion object {
        val storageAccess = Permissions(arrayOf(Manifest.permission.MANAGE_EXTERNAL_STORAGE))
    }
}