package com.example.mymeds.ui.takesList

import android.view.LayoutInflater
import android.view.ViewGroup
import androidx.recyclerview.widget.RecyclerView
import com.example.mymeds.data.model.Take
import com.example.mymeds.databinding.ItemTakeBinding

class TakesAdapter (
    private val takes: List<Take>,
    private val onEditClick: (Int) -> Unit,
    private val onDeleteClick: (Int) -> Unit
) : RecyclerView.Adapter<TakesAdapter.TakeViewHolder>(){

    inner class TakeViewHolder(
        val binding: ItemTakeBinding
    ) : RecyclerView.ViewHolder(binding.root)

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): TakeViewHolder {
        val binding = ItemTakeBinding.inflate(LayoutInflater.from(parent.context), parent, false)

        return TakeViewHolder(binding)
    }

    override fun onBindViewHolder(holder: TakeViewHolder, position: Int) {
        val take = takes[position]

        holder.binding.textTime.text = take.time

        holder.binding.textDays.text = take.days.joinToString(" ") { it.label }

        holder.binding.textReminder.text =
            if (take.reminderEnabled)
                "Recordatorio activado"
            else
                "Recordatorio desactivado"

        holder.binding.buttonEdit.setOnClickListener {
            onEditClick(position)
        }

        holder.binding.buttonDelete.setOnClickListener {
            onDeleteClick(position)
        }
    }

    override fun getItemCount(): Int = takes.size
}