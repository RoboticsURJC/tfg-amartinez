package com.example.mymeds.ui.takesConfig

import android.app.TimePickerDialog
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ArrayAdapter
import android.widget.Toast
import androidx.fragment.app.Fragment
import androidx.navigation.fragment.findNavController
import androidx.recyclerview.widget.LinearLayoutManager
import com.example.mymeds.R
import com.example.mymeds.data.model.Medicine
import com.example.mymeds.databinding.FragmentTakesConfigBinding
import com.google.android.material.chip.Chip
import java.util.Calendar

class TakesConfigFragment : Fragment() {

    private var _binding: FragmentTakesConfigBinding? = null
    private val binding get() = _binding!!

    private lateinit var chips: List<Chip>

    // Medicamentos
    private val medicines = mutableListOf<Medicine>()
    private lateinit var medicineAdapter: MedicineAdapter

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentTakesConfigBinding.inflate(inflater, container, false)

        // Volver
        binding.buttonBack.setOnClickListener {
            findNavController().popBackStack()
        }

        // Guardar toma
        binding.buttonSave.setOnClickListener {

            // Hora
            if (binding.timeInput.text.isNullOrBlank()) {
                binding.timeLayout.error = "Selecciona una hora"
                return@setOnClickListener
            } else {
                binding.timeLayout.error = null
            }

            // Días
            val selectedDays = chips.filter { it.isChecked }
            if (selectedDays.isEmpty()) {
                Toast.makeText(requireContext(), "Selecciona al menos un día", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }

            // Medicamentos
            if (medicines.isEmpty()) {
                Toast.makeText(requireContext(), "Añade al menos un medicamento", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }

            val invalidMedicine = medicines.any {
                it.name.isBlank() || it.quantity.isBlank()
            }

            if (invalidMedicine) {
                Toast.makeText(requireContext(), "Completa al menos un medicamento", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }

            // Simulación guardado
            Toast.makeText(requireContext(), "Toma guardada (simulado)", Toast.LENGTH_SHORT).show()
            findNavController().popBackStack()
        }

        // Selector hora
        binding.timeInput.setOnClickListener {
            val now = Calendar.getInstance()
            TimePickerDialog(
                requireContext(),
                { _, hour, minute ->
                    binding.timeInput.setText(String.format("%02d:%02d", hour, minute))
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

        // Chips días
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

        // Recycler medicamentos
        medicineAdapter = MedicineAdapter(medicines) { position ->
            medicines.removeAt(position)
            medicineAdapter.notifyItemRemoved(position)
        }

        binding.medicinesRecycler.apply {
            layoutManager = LinearLayoutManager(requireContext())
            adapter = medicineAdapter
        }

        // Añadir medicamento
        binding.buttonMedicine.setOnClickListener {
            medicineAdapter.addMedicine(Medicine())
        }

        // Dropdown aviso previo
        val options = listOf("5 minutos", "10 minutos", "15 minutos", "20 minutos", "30 minutos")
        val adapter = ArrayAdapter(requireContext(), android.R.layout.simple_list_item_1, options)
        binding.warningDropdown.setAdapter(adapter)
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}
