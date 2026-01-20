package com.example.mymeds.ui.takesConfig

import android.app.TimePickerDialog
import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ArrayAdapter
import android.widget.Toast
import androidx.navigation.fragment.findNavController
import com.example.mymeds.R
import com.example.mymeds.databinding.FragmentTakesConfigBinding
import java.util.Calendar

class TakesConfigFragment : Fragment() {

    private var _binding: FragmentTakesConfigBinding? = null
    private val binding get() = _binding!!

    private lateinit var chips: List<com.google.android.material.chip.Chip>

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentTakesConfigBinding.inflate(inflater, container, false)

        binding.buttonBack.setOnClickListener {
            findNavController().popBackStack()
        }

        binding.buttonSave.setOnClickListener {

            if (binding.timeInput.text.isNullOrBlank()) {
                binding.timeLayout.error = "Selecciona una hora"
                return@setOnClickListener
            } else {
                binding.timeLayout.error = null
            }

            if (binding.medicinesInput.text.isNullOrBlank()) {
                binding.medicinesLayout.error = "Introduce al menos un medicamento"
                return@setOnClickListener
            } else {
                binding.medicinesLayout.error = null
            }

            if (binding.quantityInput.text.isNullOrBlank()) {
                binding.quantityLayout.error = "Indica la cantidad"
                return@setOnClickListener
            } else {
                binding.quantityLayout.error = null
            }

            val selectedDays = chips.filter { it.isChecked }
            if (selectedDays.isEmpty()) {
                Toast.makeText(
                    requireContext(),
                    "Selecciona al menos un día",
                    Toast.LENGTH_SHORT
                ).show()
                return@setOnClickListener
            }

            Toast.makeText(
                requireContext(),
                "Toma guardada (simulado)",
                Toast.LENGTH_SHORT
            ).show()

            findNavController().popBackStack()
        }

        binding.timeInput.setOnClickListener {
            val now = Calendar.getInstance()
            TimePickerDialog(
                requireContext(),
                { _, hour, minute ->
                    binding.timeInput.setText(
                        String.format("%02d:%02d", hour, minute)
                    )
                },
                now.get(Calendar.HOUR_OF_DAY),
                now.get(Calendar.MINUTE),
                true
            ).show()
        }

        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        // Inicialización REAL de chips
        chips = listOf(
            binding.chipMonday,
            binding.chipTuesday,
            binding.chipWednesday,
            binding.chipThursday,
            binding.chipFriday,
            binding.chipSaturday,
            binding.chipSunday
        )

        chips.forEach { chip ->
            chip.isCheckable = true

            chip.setOnCheckedChangeListener { _, isChecked ->
                if (isChecked) {
                    chip.setChipBackgroundColorResource(R.color.purple_500)
                    chip.setTextColor(resources.getColor(R.color.white, null))
                } else {
                    chip.setChipBackgroundColorResource(R.color.purple_200)
                    chip.setTextColor(resources.getColor(R.color.black, null))
                }
            }
        }

        // Dropdown aviso previo
        val options = listOf(
            "5 minutos",
            "10 minutos",
            "15 minutos",
            "20 minutos",
            "30 minutos"
        )

        val adapter = ArrayAdapter(
            requireContext(),
            android.R.layout.simple_list_item_1,
            options
        )

        binding.warningDropdown.setAdapter(adapter)
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}
