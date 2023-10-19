package emu.zenith.helper.views

import android.view.ViewGroup
import emu.zenith.adapter.GenericListContainer
import emu.zenith.adapter.GenericViewHolder
import emu.zenith.adapter.ViewBindingFactory
import emu.zenith.adapter.inflater
import emu.zenith.data.KernelModel
import emu.zenith.databinding.KernelItemBinding

object KernelBindingFactory : ViewBindingFactory {
    override fun create(parent: ViewGroup) =
        KernelItemBinding.inflate(parent.inflater(), parent, false)
}

class KernelViewItem(private val model: KernelModel) : GenericListContainer<KernelItemBinding>() {
    override fun getFactory(): ViewBindingFactory = KernelBindingFactory

    override fun bind(holder: GenericViewHolder<KernelItemBinding>, position: Int) {
        val binding = holder.binding
        binding.kernelName.text = model.kName
        binding.kernelObject.text = model.kObject
        binding.kernelOriginVersion.text = model.originVersion

        binding.kernelChecker.apply {
            isChecked = model.selected
        }
    }

    override fun compareItem(prob: GenericListContainer<KernelItemBinding>): Boolean
        = prob is KernelViewItem &&
            prob.model.dataCRC == model.dataCRC &&
            prob.model.kObject == model.kObject

    override fun isTheSame(prob: GenericListContainer<KernelItemBinding>): Boolean
        = prob is KernelViewItem && prob.model == model
}