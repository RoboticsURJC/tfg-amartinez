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

        holder.binding.textTime.text = "🕒 ${take.time}"

        holder.binding.textDays.text = "📅 " + take.days.joinToString(" ") { it.label }

        val meds = take.medicines ?: emptyList()

        holder.binding.textMedicines.text =
            if (meds.isNotEmpty()) {

                "💊 " + meds.joinToString("\n") { med ->

                    val realMedicine =
                        com.example.mymeds.data.repository.MedicineRepository
                            .findById(med.id)

                    val medName =
                        realMedicine?.name ?: "Desconocido"

                    "$medName - ${med.quantity}"
                }

            } else {

                "💊 Sin medicamentos"
            }

        if (take.reminderEnabled) {

            val warningText =
                take.advanceWarningMinutes?.let {
                    "Aviso previo: $it min"
                } ?: "Sin aviso previo"

            holder.binding.textReminder.text =
                "🔔 Recordatorio activado\n$warningText"

        } else {

            holder.binding.textReminder.text =
                "🔕 Recordatorio desactivado"
        }

        holder.binding.buttonEdit.setOnClickListener {
            onEditClick(position)
        }

        holder.binding.buttonDelete.setOnClickListener {
            onDeleteClick(position)
        }
    }

    override fun getItemCount(): Int = takes.size
}