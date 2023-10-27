package emu.zenith.views

import android.annotation.SuppressLint
import android.content.Context
import android.view.View
import android.view.ViewGroup
import androidx.viewbinding.ViewBinding
import emu.zenith.adapters.GenericListContainer
import emu.zenith.adapters.GenericViewHolder
import emu.zenith.adapters.ViewBindingFactory
import emu.zenith.adapters.inflater
import emu.zenith.data.DriverMeta
import emu.zenith.databinding.DriverItemBinding


object DriverBindFactory : ViewBindingFactory {
    override fun create(parent: ViewGroup): ViewBinding = DriverItemBinding.inflate(parent.inflater(), parent, false)
}

class DriverViewItem(
    private val context: Context,
    private val driver: DriverMeta,
    var onDelete: ((position: Int, used: Boolean) -> Unit)? = null,
    var onClick: ((View) -> Unit)? = null)
    : GenericListContainer<DriverItemBinding>() {

    private lateinit var binding: DriverItemBinding

    override fun getFactory(): ViewBindingFactory = DriverBindFactory

    @SuppressLint("SetTextI18n")
    override fun bind(holder: GenericViewHolder<DriverItemBinding>, position: Int) {
        binding = holder.binding
        binding.drvName.text = "Name: ${driver.name}"
        binding.drvDescription.text = "Desc: ${driver.description}"
        binding.drvAuthor.text = "Author: ${driver.author}"
        binding.drvVendor.text = "Vendor: ${driver.vendor}"
        binding.drvMinApi.text = "MinAPI: ${driver.minApi}"
        binding.dpkVulkanLib.text = "Library Name: ${driver.libraryName}"

        onClick?.let {
            binding.drvChecker.setOnClickListener(it)
            binding.root.setOnClickListener(it)
        }
    }

    override fun compareItem(prob: GenericListContainer<DriverItemBinding>): Boolean {
        return if (prob is DriverViewItem)
            prob.driver.name == driver.name && prob.driver.libraryName == driver.libraryName
        else
            false
    }

    override fun isTheSame(prob: GenericListContainer<DriverItemBinding>) =
        prob is DriverViewItem && prob.driver == driver
}