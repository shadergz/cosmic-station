package emu.cosmic.views

import android.annotation.SuppressLint
import android.content.Context
import android.view.View
import android.view.ViewGroup
import androidx.viewbinding.ViewBinding
import emu.cosmic.adapters.GenericListContainer
import emu.cosmic.adapters.GenericViewHolder
import emu.cosmic.adapters.ViewBindingFactory
import emu.cosmic.adapters.inflater
import emu.cosmic.databinding.DriverItemBinding
import emu.cosmic.helpers.DriverContainer


object DriverBindFactory : ViewBindingFactory {
    override fun create(parent: ViewGroup): ViewBinding = DriverItemBinding.inflate(parent.inflater(), parent, false)
}

class DriverViewItem(
    private val context: Context,
    private val driver: DriverContainer,
    var onDelete: ((position: Int, used: Boolean) -> Unit)? = null,
    var onClick: ((View) -> Unit)? = null)
    : GenericListContainer<DriverItemBinding>() {

    private lateinit var binding: DriverItemBinding

    override fun getFactory(): ViewBindingFactory = DriverBindFactory

    @SuppressLint("SetTextI18n")
    override fun bind(holder: GenericViewHolder<DriverItemBinding>, position: Int) {
        binding = holder.binding
        val meta = driver.meta
        binding.drvName.text = "Name: ${meta.name}"
        binding.drvDescription.text = "Desc: ${meta.description}"
        binding.drvAuthor.text = "Author: ${meta.author}"
        binding.drvVendor.text = "Vendor: ${meta.vendor}"
        binding.drvMinApi.text = "MinAPI: ${meta.minApi}"
        binding.dpkVulkanLib.text = "Library Name: ${meta.libraryName}"

        binding.drvChecker.isChecked = driver.selected

        onClick?.let {
            binding.drvChecker.setOnClickListener(it)
            binding.root.setOnClickListener(it)
        }
    }

    override fun compareItem(prob: GenericListContainer<DriverItemBinding>): Boolean {
        val metaProb = (prob as DriverViewItem).let {
            prob.driver.meta
        }
        val metaOrigin = driver.meta
        return metaProb.name == metaOrigin.name && metaProb.libraryName == metaOrigin.libraryName
    }

    override fun isTheSame(prob: GenericListContainer<DriverItemBinding>) =
        prob is DriverViewItem && prob.driver == driver
}