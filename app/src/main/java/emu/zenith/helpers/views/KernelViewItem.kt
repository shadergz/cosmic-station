package emu.zenith.helpers.views

import android.content.Context
import android.graphics.BitmapFactory
import android.view.View
import android.view.ViewGroup
import emu.zenith.adapters.GenericListContainer
import emu.zenith.adapters.GenericViewHolder
import emu.zenith.adapters.ViewBindingFactory
import emu.zenith.adapters.inflater
import emu.zenith.data.BiosModel
import emu.zenith.databinding.BiosItemBinding

object KernelBindingFactory : ViewBindingFactory {
    override fun create(parent: ViewGroup) =
        BiosItemBinding.inflate(parent.inflater(), parent, false)
}

class KernelViewItem(
    private val context: Context,
    private val model: BiosModel,
    var onDelete: ((position: Int, used: Boolean) -> Unit)? = null,
    var onClick: ((View) -> Unit)? = null)
    : GenericListContainer<BiosItemBinding>() {

    override fun getFactory(): ViewBindingFactory = KernelBindingFactory

    override fun bind(holder: GenericViewHolder<BiosItemBinding>, position: Int) {
        val binding = holder.binding

        binding.biosFullQualified.text = model.biosFilename
        binding.biosName.text = model.biosName
        binding.biosDetails.text = model.biosDetails

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
            binding.biosFlag.setImageBitmap(bitmap)
        }

        binding.biosChecker.apply {
            isChecked = model.selected
        }
        onClick?.let {
            binding.biosChecker.setOnClickListener(it)
            binding.root.setOnClickListener(it)
        }
    }

    override fun compareItem(prob: GenericListContainer<BiosItemBinding>): Boolean
        = prob is KernelViewItem &&
            prob.model.position == model.position &&
            prob.model.biosName == model.biosName

    override fun isTheSame(prob: GenericListContainer<BiosItemBinding>): Boolean
        = prob is KernelViewItem && prob.model == model
}