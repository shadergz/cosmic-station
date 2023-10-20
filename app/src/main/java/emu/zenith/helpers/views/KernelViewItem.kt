package emu.zenith.helpers.views

import android.content.Context
import android.graphics.BitmapFactory
import android.view.ViewGroup
import emu.zenith.adapters.GenericListContainer
import emu.zenith.adapters.GenericViewHolder
import emu.zenith.adapters.SelectableViewAdapter
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
    private var onDelete : ((position : Int, wasChecked : Boolean) -> Unit)? = null,
    private var onClick : (() -> Unit)? = null) : GenericListContainer<KernelItemBinding>() {

    override fun getFactory(): ViewBindingFactory = KernelBindingFactory

    override fun bind(holder: GenericViewHolder<KernelItemBinding>, position: Int) {
        val binding = holder.binding
        binding.kernelFullQualified.text = model.biosFilename
        binding.kernelName.text = model.biosName
        binding.kernelDetails.text = model.biosDetails

        val flag = when (model.biosName?.substringBefore(' ')) {
            "USA" -> "countries/us.png"
            "Japan" -> "countries/jp.png"
            "Europe" -> "countries/eu.png"
            "China" -> "countries/ch.png"
            "Honk Kong" -> "countries/jp.png"
            else -> ""
        }

        val bitmap = BitmapFactory.decodeStream(context.assets.open(flag))
        binding.kernelFlag.setImageBitmap(bitmap)

        binding.kernelChecker.apply {
            isChecked = model.selected
        }

        binding.root.setOnClickListener {
            (adaptedBy as SelectableViewAdapter).selectItem(position)
            onClick?.invoke()
        }
        onDelete?.let {
            var wasChecked: Boolean
            (adaptedBy as SelectableViewAdapter).apply {
                dropItemAt(position)
                wasChecked = position == selectedPos
            }
            it.invoke(position, wasChecked)
        }
    }

    override fun compareItem(prob: GenericListContainer<KernelItemBinding>): Boolean
        = prob is KernelViewItem &&
            prob.model.dataCRC == model.dataCRC &&
            prob.model.biosName == model.biosName

    override fun isTheSame(prob: GenericListContainer<KernelItemBinding>): Boolean
        = prob is KernelViewItem && prob.model == model
}