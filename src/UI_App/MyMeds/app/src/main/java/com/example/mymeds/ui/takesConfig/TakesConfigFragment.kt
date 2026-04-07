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
import com.example.mymeds.data.model.DayOfWeek
import com.example.mymeds.data.model.Take
import com.example.mymeds.data.repository.TakeRepository
import com.example.mymeds.data.repository.EspConfig
import com.example.mymeds.data.util.JsonUtils
import android.util.Log

class TakesConfigFragment : Fragment() {

    private var _binding: FragmentTakesConfigBinding? = null
    private val binding get() = _binding!!

    private lateinit var chips: List<Chip>

    // Medicamentos
    private val medicines = mutableListOf<Medicine>()
    private lateinit var medicineAdapter: MedicineAdapter

    private var editPosition: Int = -1

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

        binding.buttonSave.setOnClickListener {

            val time = binding.timeInput.text.toString()

            // --- Validación hora ---
            if (time.isBlank()) {
                binding.timeLayout.error = "Selecciona una hora"
                return@setOnClickListener
            } else {
                binding.timeLayout.error = null
            }

            // --- Días seleccionados ---
            val selectedDays = chips
                .filter { it.isChecked }
                .map { chip ->
                    when (chip.id) {
                        R.id.chipMonday -> DayOfWeek.MONDAY
                        R.id.chipTuesday -> DayOfWeek.TUESDAY
                        R.id.chipWednesday -> DayOfWeek.WEDNESDAY
                        R.id.chipThursday -> DayOfWeek.THURSDAY
                        R.id.chipFriday -> DayOfWeek.FRIDAY
                        R.id.chipSaturday -> DayOfWeek.SATURDAY
                        R.id.chipSunday -> DayOfWeek.SUNDAY
                        else -> null
                    }
                }
                .filterNotNull()

            if (selectedDays.isEmpty()) {
                Toast.makeText(requireContext(), "Selecciona al menos un día", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }

            // --- Medicamentos ---
            if (medicines.isEmpty()) {
                Toast.makeText(requireContext(), "Añade al menos un medicamento", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }

            val invalidMedicine = medicines.any {
                it.name.isBlank() || it.quantity.isBlank()
            }

            if (invalidMedicine) {
                Toast.makeText(requireContext(), "Completa todos los medicamentos", Toast.LENGTH_SHORT).show()
                return@setOnClickListener
            }

            // --- Recordatorio ---
            val reminderEnabled = binding.reminderSwitch.isChecked

            val advanceWarningMinutes = if (binding.warningDropdown.text.isNullOrBlank()) {
                null
            } else {
                binding.warningDropdown.text.toString()
                    .substringBefore(" ")
                    .toIntOrNull()
            }

            // --- Crear objeto Take ---
            val take = Take(
                time = time,
                days = selectedDays,
                medicines = medicines.toList(),
                reminderEnabled = reminderEnabled,
                advanceWarningMinutes = advanceWarningMinutes
            )

            val json = JsonUtils.takeToJson(take)

            Log.d("JSON_TAKE", json)

            // --- Guardar en repositorio ---
            if (editPosition == -1) {

                TakeRepository.addTake(take)

            } else {

                TakeRepository.updateTake(
                    editPosition,
                    take
                )
            }

            sendTakesToEsp()

            Toast.makeText(
                requireContext(),
                "Toma guardada correctamente",
                Toast.LENGTH_SHORT
            ).show()

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

        if (arguments != null && requireArguments().containsKey("position")) {

            editPosition = requireArguments().getInt("position")

        } else {

            editPosition = -1
        }

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

        if (editPosition != -1) {

            val take = TakeRepository.getTakes()[editPosition]

            // Hora
            binding.timeInput.setText(take.time)

            // Días
            chips.forEach { chip ->
                chip.isChecked =
                    take.days.any { it.label == chip.text.toString() }
            }

            // Medicamentos
            medicines.clear()
            medicines.addAll(take.medicines)
            medicineAdapter.notifyDataSetChanged()

            // Recordatorio
            binding.reminderSwitch.isChecked =
                take.reminderEnabled

            // Warning
            take.advanceWarningMinutes?.let {
                binding.warningDropdown.setText(
                    "$it minutos",
                    false
                )
            }
        }
    }

    private fun sendTakesToEsp() {

        Log.d("ESP_URL", EspConfig.baseUrl)
        val takes = TakeRepository.getTakes()

        Thread {

            try {

                val url = java.net.URL(
                    "http://192.168.1.18/takes"
                    //EspConfig.baseUrl + "/takes"
                )

                val conn =
                    url.openConnection() as java.net.HttpURLConnection

                conn.requestMethod = "POST"
                conn.doOutput = true
                conn.setRequestProperty(
                    "Content-Type",
                    "application/json"
                )

                val json = JsonUtils.takesToJson(takes)

                val out = conn.outputStream
                out.write(json.toByteArray())
                out.flush()
                out.close()

                conn.responseCode

            } catch (e: Exception) {
                e.printStackTrace()
            }

        }.start()
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}
