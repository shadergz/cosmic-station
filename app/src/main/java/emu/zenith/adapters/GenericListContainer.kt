package emu.zenith.adapters

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.viewbinding.ViewBinding

interface ViewBindingFactory {
    fun create(parent: ViewGroup): ViewBinding
}
fun View.inflater() = LayoutInflater.from(context)!!

abstract class GenericListContainer <T : ViewBinding> {
    var adaptedBy: GenericViewAdapter? = null

    abstract fun getFactory(): ViewBindingFactory
    abstract fun bind(holder: GenericViewHolder<T>, position: Int)

    abstract fun compareItem(prob: GenericListContainer<T>): Boolean
    abstract fun isTheSame(prob: GenericListContainer<T>): Boolean
}
