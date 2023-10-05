package emu.zenith.adapter

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.recyclerview.widget.RecyclerView
import androidx.recyclerview.widget.RecyclerView.ViewHolder

class AdaptableListHolder(itemView: View) : ViewHolder(itemView)
@Suppress("unused")
class GenericViewAdapter : RecyclerView.Adapter<AdaptableListHolder>() {
    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): AdaptableListHolder {
        val view = LayoutInflater.from(parent.context).inflate(
            0, parent, false)
        return AdaptableListHolder(view)
    }

    override fun getItemCount(): Int {
        TODO("Not yet implemented")
    }

    override fun onBindViewHolder(holder: AdaptableListHolder, position: Int) {
        TODO("Not yet implemented")
    }
}
