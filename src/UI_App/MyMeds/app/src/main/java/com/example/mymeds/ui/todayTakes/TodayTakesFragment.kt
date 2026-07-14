package com.example.mymeds.ui.todayTakes

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import com.example.mymeds.databinding.FragmentTodayTakesBinding
import androidx.recyclerview.widget.LinearLayoutManager
import com.example.mymeds.data.repository.TakeRepository
import androidx.navigation.fragment.findNavController
import android.content.Context
import com.example.mymeds.data.util.JsonUtils

class TodayTakesFragment : Fragment() {

    private var _binding: FragmentTodayTakesBinding? = null
    private val binding get() = _binding!!

    override fun onViewCreated(
        view: View,
        savedInstanceState: Bundle?
    ) {
        super.onViewCreated(view, savedInstanceState)

        loadTakesFromPrefs()

        binding.buttonBack.setOnClickListener {

            findNavController().navigateUp()
        }

        val calendar =
            java.util.Calendar.getInstance()

        val day =
            calendar.get(java.util.Calendar.DAY_OF_WEEK)

        val today = when (day) {

            java.util.Calendar.MONDAY -> "MONDAY"
            java.util.Calendar.TUESDAY -> "TUESDAY"
            java.util.Calendar.WEDNESDAY -> "WEDNESDAY"
            java.util.Calendar.THURSDAY -> "THURSDAY"
            java.util.Calendar.FRIDAY -> "FRIDAY"
            java.util.Calendar.SATURDAY -> "SATURDAY"
            else -> "SUNDAY"
        }

        val todayTakes =
            TakeRepository.getTakes()
                .filter {
                    it.days.any { day ->
                        day.name.equals(today, true)
                    }
                }
                .sortedBy { it.time }

        val currentMinutes =
            calendar.get(java.util.Calendar.HOUR_OF_DAY) * 60 +
                    calendar.get(java.util.Calendar.MINUTE)

        val nextIndex =
            todayTakes.indexOfFirst {

                val parts = it.time.split(":")

                val takeMinutes =
                    parts[0].toInt() * 60 +
                            parts[1].toInt()

                takeMinutes >= currentMinutes
            }

        binding.todayRecycler.layoutManager =
            LinearLayoutManager(requireContext())

        binding.todayRecycler.adapter =
            TodayTakesAdapter(
                todayTakes,
                nextIndex
            )
    }

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {

        _binding = FragmentTodayTakesBinding.inflate(
            inflater,
            container,
            false
        )

        return binding.root
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }

    private fun loadTakesFromPrefs() {

        val prefs = requireContext()
            .getSharedPreferences(
                "app",
                Context.MODE_PRIVATE
            )

        val json = prefs.getString("takes", null)

        if (json != null) {

            val takes =
                JsonUtils.jsonToTakes(json)

            TakeRepository.clear()
            TakeRepository.addAll(takes)
        }
    }
}