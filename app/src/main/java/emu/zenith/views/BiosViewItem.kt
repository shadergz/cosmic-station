package emu.zenith.views

import android.content.Context
import android.graphics.BitmapFactory
import android.view.View
import android.view.ViewGroup
import emu.zenith.adapters.GenericListContainer
import emu.zenith.adapters.GenericViewHolder
import emu.zenith.adapters.ViewBindingFactory
import emu.zenith.adapters.inflater
import emu.zenith.data.BiosInfo
import emu.zenith.databinding.BiosItemBinding

object BiosBindingFactory : ViewBindingFactory {
    override fun create(parent: ViewGroup) =
        BiosItemBinding.inflate(parent.inflater(), parent, false)
}

class BiosViewItem(
    private val context: Context,
    private val bios: BiosInfo,
    var onDelete: ((position: Int, used: Boolean) -> Unit)? = null,
    var onClick: ((View) -> Unit)? = null)
    : GenericListContainer<BiosItemBinding>() {

    override fun getFactory(): ViewBindingFactory = BiosBindingFactory

    override fun bind(holder: GenericViewHolder<BiosItemBinding>, position: Int) {
        val binding = holder.binding

        binding.biosFullQualified.text = bios.biosFilename
        binding.biosName.text = bios.biosName
        binding.biosDetails.text = bios.biosDetails

        val flag = when (bios.biosName.substringBefore(' ')) {
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
            isChecked = bios.selected
        }
        onClick?.let {
            binding.biosChecker.setOnClickListener(it)
            binding.root.setOnClickListener(it)
        }
    }

    override fun compareItem(prob: GenericListContainer<BiosItemBinding>): Boolean
        = prob is BiosViewItem &&
            prob.bios.position == bios.position &&
            prob.bios.biosName == bios.biosName

    override fun isTheSame(prob: GenericListContainer<BiosItemBinding>): Boolean
        = prob is BiosViewItem && prob.bios == bios
}