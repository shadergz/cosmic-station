package emu.zenith.adapter

import android.view.View
import android.view.ViewGroup
import androidx.recyclerview.widget.AsyncListDiffer
import androidx.recyclerview.widget.DiffUtil
import androidx.recyclerview.widget.RecyclerView
import androidx.recyclerview.widget.RecyclerView.ViewHolder
import androidx.viewbinding.ViewBinding

class AdaptableListHolder(view: View) : ViewHolder(view)

class GenericViewHolder<out T: ViewBinding>(view : T) : ViewHolder(view.root) {
    val binding = view
}

@Suppress("LeakingThis")
open class GenericViewAdapter
    : RecyclerView.Adapter<GenericViewHolder<ViewBinding>>() {
    companion object {
        private val viewDiffer = object : DiffUtil.ItemCallback<GenericListContainer<ViewBinding>>() {
            override fun areItemsTheSame(obj: GenericListContainer<ViewBinding>,
                prob: GenericListContainer<ViewBinding>) = obj.compareItem(prob)
            override fun areContentsTheSame(obj: GenericListContainer<ViewBinding>,
                prob: GenericListContainer<ViewBinding>) = obj.isTheSame(prob)
        }
    }

    private val asyncDiffer = AsyncListDiffer(this, viewDiffer)

    private val entities = mutableListOf<GenericListContainer<out ViewBinding>>()

    private val viewItems: List<GenericListContainer<in ViewBinding>> get() = asyncDiffer.currentList

    private val factory = mutableMapOf<ViewBindingFactory, Int>()

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): GenericViewHolder<ViewBinding>
        = GenericViewHolder(factory.filterValues { it == viewType }.keys.single().create(parent))

    fun feedAdapter(itemList : List<GenericListContainer<*>>) {
        entities.clear()
        entities.addAll(itemList)
    }
    open fun rmItemAt(position: Int) {
        entities.removeAt(position)
    }
    open fun addItemAt(item: GenericListContainer<ViewBinding>, position: Int) {
        entities.add(position, item)
    }

    override fun getItemViewType(position: Int)
        = factory.getOrPut(viewItems[position].getFactory()) { factory.size }

    override fun onBindViewHolder(holder: GenericViewHolder<ViewBinding>, position: Int) {
        viewItems[position].apply {
            adaptedBy = this@GenericViewAdapter
            bind(holder, position)
        }
    }
    override fun getItemCount(): Int {
        return entities.size
    }
}
