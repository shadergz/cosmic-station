package emu.zenith.adapters

import androidx.viewbinding.ViewBinding

class SelectableViewAdapter(private val defaultPos: Int) : GenericViewAdapter() {
    var selectedPos = defaultPos

    fun selectItem(position: Int) {
        if (selectedPos != position) {
            notifyItemChanged(position)
            notifyItemChanged(selectedPos)
        }
        selectedPos = position
    }

    fun removeItemAt(position: Int) {
        dropItemAt(position)
        if (position < selectedPos)
            selectedPos--
        else if (position == selectedPos)
            selectItem(defaultPos)
        notifyItemRemoved(position)
    }
    override fun putItemAt(item: GenericListContainer<ViewBinding>, position: Int) {
        putItemAt(item, position)
        notifyItemInserted(position)
    }
}