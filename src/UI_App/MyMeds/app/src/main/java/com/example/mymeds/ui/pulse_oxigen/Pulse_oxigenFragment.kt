package com.example.mymeds.ui.pulse_oxigen

import androidx.fragment.app.viewModels
import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.example.mymeds.R
import android.util.Log
import com.example.mymeds.databinding.FragmentConfigPillsBinding
import com.example.mymeds.databinding.FragmentPulseOxigenBinding
import com.example.mymeds.data.network.EspApi
import com.example.mymeds.data.PulseMeasurement
import com.example.mymeds.repository.PulseHistoryRepository
import org.json.JSONObject
import androidx.recyclerview.widget.LinearLayoutManager

class Pulse_oxigenFragment : Fragment() {

    private var _binding: FragmentPulseOxigenBinding? = null
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentPulseOxigenBinding.inflate(inflater, container, false)

        EspApi.getPulseHistory { response ->

            if (response == null) {
                return@getPulseHistory
            }

            Log.d("PULSE_HISTORY", response)

            try {

                val json = JSONObject(response)

                val measurementsArray =
                    json.getJSONArray("measurements")

                val measurements =
                    mutableListOf<PulseMeasurement>()

                for (i in 0 until measurementsArray.length())
                {
                    val item =
                        measurementsArray.getJSONObject(i)

                    measurements.add(
                        PulseMeasurement(
                            date = item.getString("date"),
                            time = item.getString("time"),
                            bpm = item.getInt("bpm")
                        )
                    )
                }

                if (measurements.isEmpty()) {
                    Log.d("PULSE_HISTORY", "No hay mediciones")
                } else {
                    Log.d("PULSE_HISTORY", "Recibidas ${measurements.size} mediciones")
                    Log.d("PULSE_HISTORY", measurements.toString())
                }

                PulseHistoryRepository.setMeasurements(
                    measurements
                )

                activity?.runOnUiThread {

                    binding.historyRecycler.layoutManager =
                        LinearLayoutManager(requireContext())

                    binding.historyRecycler.adapter =
                        PulseHistoryAdapter(
                            measurements.reversed()
                        )
                }

            } catch (e: Exception) {
                e.printStackTrace()
            }
        }

        return binding.root
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}