package emu.cosmic.fragments

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import emu.cosmic.databinding.GamesFolderFragmentBinding

class GamesFolderFragment : Fragment() {
    private val binding by lazy { GamesFolderFragmentBinding.inflate(layoutInflater) }
    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        // val layout = LinearLayoutManager(context)
        // binding.gamesFolders.layoutManager = layout
        return binding.root.rootView
    }
}