package emu.zenith.adapter

import androidx.viewbinding.ViewBinding

class SelectableViewAdapter(private val defaultPos: Int) : GenericViewAdapter() {
    private var selectedPos = defaultPos
    private fun selectItem(position: Int) {
        if (selectedPos != position) {
            notifyItemChanged(position)
            notifyItemChanged(selectedPos)
        }
        selectedPos = position
    }

    override fun rmItemAt(position: Int) {
        super.rmItemAt(position)
        if (position < selectedPos)
            selectedPos--
        else if (position == selectedPos)
            selectItem(defaultPos)
        notifyItemRemoved(position)
    }
    override fun addItemAt(item: GenericListContainer<ViewBinding>, position: Int) {
        super.addItemAt(item, position)
        notifyItemInserted(position)
    }
}