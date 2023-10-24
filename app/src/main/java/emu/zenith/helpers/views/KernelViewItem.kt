package emu.zenith.helpers.views

import android.content.Context
import android.graphics.BitmapFactory
import android.view.View
import android.view.ViewGroup
import emu.zenith.adapters.GenericListContainer
import emu.zenith.adapters.GenericViewHolder
import emu.zenith.adapters.ViewBindingFactory
import emu.zenith.adapters.inflater
import emu.zenith.data.KernelModel
import emu.zenith.databinding.KernelItemBinding

object KernelBindingFactory : ViewBindingFactory {
    override fun create(parent: ViewGroup) =
        KernelItemBinding.inflate(parent.inflater(), parent, false)
}

class KernelViewItem(
    private val context: Context,
    private val model: KernelModel,
    var onDelete: ((position: Int, used: Boolean) -> Unit)? = null,
    var onClick: ((View) -> Unit)? = null)
    : GenericListContainer<KernelItemBinding>() {

    override fun getFactory(): ViewBindingFactory = KernelBindingFactory

    override fun bind(holder: GenericViewHolder<KernelItemBinding>, position: Int) {
        val binding = holder.binding

        binding.kernelFullQualified.text = model.biosFilename
        binding.kernelName.text = model.biosName
        binding.kernelDetails.text = model.biosDetails

        val flag = when (model.biosName.substringBefore(' ')) {
            "USA" -> "countries/us.png"
            "Japan" -> "countries/jp.png"
            "Europe" -> "countries/eu.png"
            "China" -> "countries/ch.png"
            "Honk Kong" -> "countries/hk.png"
            else -> ""
        }

        if (flag.isNotEmpty()) {
            val bitmap = BitmapFactory.decodeStream(context.assets.open(flag))
            binding.kernelFlag.setImageBitmap(bitmap)
        }

        binding.kernelChecker.apply {
            isChecked = model.selected
        }
        onClick?.let {
            binding.kernelChecker.setOnClickListener(it)
            binding.root.setOnClickListener(it)
        }
    }

    override fun compareItem(prob: GenericListContainer<KernelItemBinding>): Boolean
        = prob is KernelViewItem &&
            prob.model.position == model.position &&
            prob.model.biosName == model.biosName

    override fun isTheSame(prob: GenericListContainer<KernelItemBinding>): Boolean
        = prob is KernelViewItem && prob.model == model
}