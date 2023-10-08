package emu.zenith.dialogs

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.google.android.material.bottomsheet.BottomSheetDialogFragment
import emu.zenith.R
import emu.zenith.databinding.AboutDialogBinding

class AboutDialog : BottomSheetDialogFragment() {
    private var binding: AboutDialogBinding? = null
    private val aboutBinding get() = binding!!

    companion object {
        const val DialogTag = "aboutDialog"
    }

    override fun onCreateView(inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?): View {
        binding = AboutDialogBinding.inflate(inflater)
        return aboutBinding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        aboutBinding.aboutCommitVersion.apply {
            val commitHash = String.format(resources.getString(R.string.about_commit))
            text = commitHash
        }
    }
}