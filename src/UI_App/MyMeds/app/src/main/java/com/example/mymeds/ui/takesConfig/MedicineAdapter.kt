package com.example.mymeds.ui.takesConfig

import android.view.LayoutInflater
import android.view.ViewGroup
import android.widget.ArrayAdapter
import androidx.core.widget.doAfterTextChanged
import androidx.recyclerview.widget.RecyclerView
import com.example.mymeds.data.model.TakeMedicine
import com.example.mymeds.data.repository.MedicineRepository
import com.example.mymeds.databinding.ItemMedicineBinding

class MedicineAdapter(
    private val medicines: MutableList<TakeMedicine>,
    private val onDelete: (Int) -> Unit
) : RecyclerView.Adapter<MedicineAdapter.MedicineViewHolder>() {

    inner class MedicineViewHolder(
        val binding: ItemMedicineBinding
    ) : RecyclerView.ViewHolder(binding.root)

    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): MedicineViewHolder {
        val binding = ItemMedicineBinding.inflate(
            LayoutInflater.from(parent.context),
            parent,
            false
        )
        return MedicineViewHolder(binding)
    }

    override fun onBindViewHolder(holder: MedicineViewHolder, position: Int) {

        val medicine = medicines[position]
        val context = holder.binding.root.context

        // 🔥 Obtener catálogo
        val catalog = MedicineRepository.getAll().map { it.name }

        val dropdownAdapter = ArrayAdapter(
            context,
            android.R.layout.simple_list_item_1,
            catalog
        )

        holder.binding.nameInput.setAdapter(dropdownAdapter)

        // 🔥 SETEAR VALORES
        holder.binding.nameInput.setText(medicine.name, false)
        holder.binding.quantityInput.setText(medicine.quantity)

        // 🔥 SOLO DESPLEGABLE (no editable)
        holder.binding.nameInput.inputType = 0
        holder.binding.nameInput.keyListener = null
        holder.binding.nameInput.isFocusable = false
        holder.binding.nameInput.isClickable = true

        // 🔥 ABRIR DROPDOWN AL CLICK
        holder.binding.nameInput.setOnClickListener {
            holder.binding.nameInput.showDropDown()
        }

        // 🔥 SELECCIÓN
        holder.binding.nameInput.setOnItemClickListener { parent, _, pos, _ ->
            val selected = parent.getItemAtPosition(pos) as String
            medicine.name = selected
        }

        // 🔥 CANTIDAD
        holder.binding.quantityInput.doAfterTextChanged {
            medicine.quantity = it?.toString().orEmpty()
        }

        // 🔥 ELIMINAR
        holder.binding.deleteButton.setOnClickListener {
            onDelete(holder.adapterPosition)
        }
    }

    override fun getItemCount(): Int = medicines.size

    fun addMedicine(medicine: TakeMedicine) {
        medicines.add(medicine)
        notifyItemInserted(medicines.size - 1)
    }
}