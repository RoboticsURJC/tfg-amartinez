package com.example.mymeds.ui.takesConfig

import android.view.LayoutInflater
import android.view.ViewGroup
import androidx.core.widget.doAfterTextChanged
import androidx.recyclerview.widget.RecyclerView
import com.example.mymeds.data.model.Medicine
import com.example.mymeds.databinding.ItemMedicineBinding

class MedicineAdapter (
    private val medicines: MutableList<Medicine>,
    private val onDelete: (Int) -> Unit
) : RecyclerView.Adapter<MedicineAdapter.MedicineViewHolder>() {

    inner class MedicineViewHolder(
        val binding: ItemMedicineBinding
    ) : RecyclerView.ViewHolder(binding.root)

    override fun onCreateViewHolder(
        parent: ViewGroup,
        viewType: Int
    ): MedicineViewHolder {

        val binding = ItemMedicineBinding.inflate(
            LayoutInflater.from(parent.context),
            parent,
            false
        )
        return MedicineViewHolder(binding)
    }

    override fun onBindViewHolder(
        holder: MedicineViewHolder,
        position: Int
    ) {
        val medicine = medicines[position]

        holder.binding.nameInput.setText(medicine.name)
        holder.binding.quantityInput.setText(medicine.quantity)

        holder.binding.nameInput.doAfterTextChanged {
            medicine.name = it?.toString().orEmpty()
        }

        holder.binding.quantityInput.doAfterTextChanged {
            medicine.quantity = it?.toString().orEmpty()
        }

        holder.binding.deleteButton.setOnClickListener {
            onDelete(position)
        }
    }

    override fun getItemCount(): Int = medicines.size

    fun addMedicine(medicine: Medicine) {
        medicines.add(medicine)
        notifyItemInserted(medicines.size - 1)
    }
}