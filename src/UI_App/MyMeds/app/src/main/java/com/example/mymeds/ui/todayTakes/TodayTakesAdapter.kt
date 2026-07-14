package com.example.mymeds.ui.todayTakes

import android.view.LayoutInflater
import android.view.ViewGroup
import androidx.recyclerview.widget.RecyclerView
import com.example.mymeds.data.model.Take
import com.example.mymeds.data.repository.MedicineRepository
import com.example.mymeds.databinding.ItemTodayTakeBinding
import android.view.View

class TodayTakesAdapter(
    private val takes: List<Take>,
    private val nextIndex: Int
) : RecyclerView.Adapter<TodayTakesAdapter.ViewHolder>() {

    inner class ViewHolder(
        val binding: ItemTodayTakeBinding
    ) : RecyclerView.ViewHolder(binding.root)

    override fun onCreateViewHolder(
        parent: ViewGroup,
        viewType: Int
    ): ViewHolder {

        val binding =
            ItemTodayTakeBinding.inflate(
                LayoutInflater.from(parent.context),
                parent,
                false
            )

        return ViewHolder(binding)
    }

    override fun onBindViewHolder(
        holder: ViewHolder,
        position: Int
    ) {

        val take = takes[position]

        holder.binding.textNext.visibility =
            if (position == nextIndex)
                View.VISIBLE
            else
                View.GONE

        holder.binding.textTime.text =
            take.time

        holder.binding.textMedicines.text =
            take.medicines.joinToString("\n") {

                val med =
                    MedicineRepository.findById(it.id)

                "${med?.name ?: "Desconocido"} x${it.quantity}"
            }

        holder.binding.textReminder.text =
            if (take.reminderEnabled)
                "🔔 Recordatorio activo"
            else
                "🔕 Sin recordatorio"

        if ((take.advanceWarningMinutes ?: 0) > 0) {

            holder.binding.textWarning.visibility =
                View.VISIBLE

            holder.binding.textWarning.text =
                "⏰ Aviso previo: ${take.advanceWarningMinutes ?: 0} min"

        } else {

            holder.binding.textWarning.visibility =
                View.GONE
        }

        holder.binding.textDays.text =
            "📅 " + take.days.joinToString(" ") {

                when (it.name) {
                    "MONDAY" -> "L"
                    "TUESDAY" -> "M"
                    "WEDNESDAY" -> "X"
                    "THURSDAY" -> "J"
                    "FRIDAY" -> "V"
                    "SATURDAY" -> "S"
                    else -> "D"
                }
            }
    }

    override fun getItemCount(): Int =
        takes.size
}