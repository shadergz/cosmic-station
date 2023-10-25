package emu.zenith.adapters

import android.annotation.SuppressLint

class SelectableViewAdapter(private val defaultPos: Int) : GenericViewAdapter() {
    var selectedPos = defaultPos

    @SuppressLint("NotifyDataSetChanged")
    fun selectItem(position: Int) {
        notifyDataSetChanged()
        selectedPos = position
    }

    fun popItem(position: Int) {
        dropItem(position)
        if (position < selectedPos)
            selectedPos--
        else if (position == selectedPos)
            selectItem(defaultPos)
        notifyItemRemoved(position)
    }

    fun insertItem(position: Int, item: GenericListContainer<*>) {
        addItem(item, position)
        notifyItemInserted(position)
    }
}