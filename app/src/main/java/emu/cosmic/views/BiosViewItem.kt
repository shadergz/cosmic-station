package emu.cosmic.views

import android.content.Context
import android.graphics.BitmapFactory
import android.view.View
import android.view.ViewGroup
import emu.cosmic.adapters.GenericListContainer
import emu.cosmic.adapters.GenericViewHolder
import emu.cosmic.adapters.ViewBindingFactory
import emu.cosmic.adapters.inflater
import emu.cosmic.data.BiosInfo
import emu.cosmic.databinding.BiosItemBinding
import java.io.File

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

        binding.biosFullQualified.text = File(bios.biosPath).name
        binding.biosName.text = bios.biosName
        binding.biosDetails.text = bios.biosDetails

        val countries = listOf(
            "countries/us.png", "countries/jp.png", "countries/eu.png", "countries/ch.png",
            "countries/hk.png")
        val flag = when (bios.biosName.substringBefore(' ')) {
            "USA" -> countries[0]
            "Japan" -> countries[1]
            "Europe" -> countries[2]
            "China" -> countries[3]
            "Honk Kong" -> countries[4]
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