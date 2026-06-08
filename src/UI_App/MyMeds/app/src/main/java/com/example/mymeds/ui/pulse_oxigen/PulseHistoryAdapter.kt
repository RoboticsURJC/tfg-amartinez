package com.example.mymeds.ui.pulse_oxigen

import android.view.LayoutInflater
import android.view.ViewGroup
import androidx.recyclerview.widget.RecyclerView
import com.example.mymeds.data.PulseMeasurement
import com.example.mymeds.databinding.ItemPulseHistoryBinding

class PulseHistoryAdapter(
    private val items: List<PulseMeasurement>
) : RecyclerView.Adapter<PulseHistoryAdapter.ViewHolder>()
{

    class ViewHolder(
        private val binding: ItemPulseHistoryBinding
    ) : RecyclerView.ViewHolder(binding.root)
    {

        fun bind(item: PulseMeasurement)
        {
            binding.txtDateTime.text =
                "${item.date} ${item.time}"

            binding.txtBpm.text =
                "${item.bpm} BPM"
        }
    }

    override fun onCreateViewHolder(
        parent: ViewGroup,
        viewType: Int
    ): ViewHolder
    {
        val binding =
            ItemPulseHistoryBinding.inflate(
                LayoutInflater.from(parent.context),
                parent,
                false
            )

        return ViewHolder(binding)
    }

    override fun onBindViewHolder(
        holder: ViewHolder,
        position: Int
    )
    {
        holder.bind(items[position])
    }

    override fun getItemCount(): Int
    {
        return items.size
    }
}