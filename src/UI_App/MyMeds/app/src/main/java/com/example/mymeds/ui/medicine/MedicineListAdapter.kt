package com.example.mymeds.ui.medicine

import android.view.LayoutInflater
import android.view.ViewGroup
import androidx.recyclerview.widget.RecyclerView
import com.example.mymeds.data.model.Medicine
import com.example.mymeds.databinding.ItemMedicineSimpleBinding

class MedicineListAdapter(
    private val medicines: List<Medicine>,
    private val onDelete: (Int) -> Unit
) : RecyclerView.Adapter<MedicineListAdapter.ViewHolder>() {

    inner class ViewHolder(val binding: ItemMedicineSimpleBinding)
        : RecyclerView.ViewHolder(binding.root)

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): ViewHolder {
        val binding = ItemMedicineSimpleBinding.inflate(
            LayoutInflater.from(parent.context),
            parent,
            false
        )
        return ViewHolder(binding)
    }

    override fun onBindViewHolder(holder: ViewHolder, position: Int) {

        val med = medicines[position]

        holder.binding.textName.text = med.name

        holder.binding.buttonDelete.setOnClickListener {
            onDelete(position)
        }
    }

    override fun getItemCount(): Int = medicines.size
}