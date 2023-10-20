package emu.zenith.helpers

import android.Manifest
import android.content.Context
import android.os.Environment
import androidx.core.content.ContextCompat
import androidx.core.content.PermissionChecker
import emu.zenith.data.Permissions

class PermissionHelper(
    val context: Context,
    private val perm: Permissions,
    private val request: (permission : Array<String>) -> Unit) {

    fun checkForPermission() {
        val perms = perm.permissions
        perms.forEach {
            when (it) {
                Manifest.permission.MANAGE_EXTERNAL_STORAGE -> {
                    if (!Environment.isExternalStorageManager())
                        return request(perms)
                }
                else -> {
                    if (ContextCompat.checkSelfPermission(context, it) !=
                        PermissionChecker.PERMISSION_GRANTED) {
                        return request(perms)
                    }
                }
            }
        }
    }
}