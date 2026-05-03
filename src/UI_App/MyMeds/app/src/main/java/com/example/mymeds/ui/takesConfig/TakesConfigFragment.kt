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
import com.example.mymeds.data.model.TakeMedicine
import com.example.mymeds.databinding.FragmentTakesConfigBinding
import com.google.android.material.chip.Chip
import java.util.Calendar
import com.example.mymeds.data.model.DayOfWeek
import com.example.mymeds.data.model.Take
import com.example.mymeds.data.repository.TakeRepository
import com.example.mymeds.data.repository.EspConfig
import com.example.mymeds.data.util.JsonUtils
import com.example.mymeds.data.network.EspApi
import android.util.Log
import android.content.Context

class TakesConfigFragment : Fragment() {

    private var _binding: FragmentTakesConfigBinding? = null
    private val binding get() = _binding!!

    private lateinit var chips: List<Chip>

    private val medicines = mutableListOf<TakeMedicine>()
    private lateinit var medicineAdapter: MedicineAdapter

    private var editPosition: Int = -1

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentTakesConfigBinding.inflate(inflater, container, false)

        binding.buttonBack.setOnClickListener {
            findNavController().popBackStack()
        }

        binding.buttonSave.setOnClickListener {

            binding.medicinesRecycler.clearFocus()

            val time = binding.timeInput.text.toString()

            if (time.isBlank()) {
                binding.timeLayout.error = "Selecciona una hora"
                return@setOnClickListener
            } else {
                binding.timeLayout.error = null
            }

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

            val reminderEnabled = binding.reminderSwitch.isChecked

            val advanceWarningMinutes = if (binding.warningDropdown.text.isNullOrBlank()) {
                null
            } else {
                binding.warningDropdown.text.toString()
                    .substringBefore(" ")
                    .toIntOrNull()
            }

            val finalMedicines = mutableListOf<TakeMedicine>()

            for (i in 0 until binding.medicinesRecycler.childCount) {

                val holder = binding.medicinesRecycler
                    .findViewHolderForAdapterPosition(i)
                        as? MedicineAdapter.MedicineViewHolder

                holder?.let {
                    val name = it.binding.nameInput.text.toString()
                    val quantity = it.binding.quantityInput.text.toString()

                    if (name.isNotBlank() || quantity.isNotBlank()) {
                        finalMedicines.add(
                            TakeMedicine(name = name, quantity = quantity)
                        )
                    }
                }
            }

            val take = Take(
                time = time,
                days = selectedDays,
                medicines = finalMedicines,
                reminderEnabled = reminderEnabled,
                advanceWarningMinutes = advanceWarningMinutes
            )

            val json = JsonUtils.takeToJson(take)
            Log.d("JSON_TAKE", json)

            if (editPosition == -1) {
                TakeRepository.addTake(take)
            } else {
                TakeRepository.updateTake(editPosition, take)
            }

            saveTakesLocally()

            // ✅ SOLO ENVÍO (SIN DISCOVERY)
            if (EspConfig.baseUrl.isEmpty()) {

                Toast.makeText(
                    requireContext(),
                    "Dispositivo no conectado",
                    Toast.LENGTH_SHORT
                ).show()

            } else {

                EspApi.sendTakes()
            }

            Toast.makeText(
                requireContext(),
                "Toma guardada correctamente",
                Toast.LENGTH_SHORT
            ).show()

            findNavController().popBackStack()
        }

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

        editPosition = if (arguments != null && requireArguments().containsKey("position")) {
            requireArguments().getInt("position")
        } else {
            -1
        }

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

        medicineAdapter = MedicineAdapter(medicines) { position ->
            medicines.removeAt(position)
            medicineAdapter.notifyItemRemoved(position)
        }

        binding.medicinesRecycler.apply {
            layoutManager = LinearLayoutManager(requireContext())
            adapter = medicineAdapter
        }

        binding.buttonMedicine.setOnClickListener {
            medicineAdapter.addMedicine(TakeMedicine())
        }

        val options = listOf("5 minutos", "10 minutos", "15 minutos", "20 minutos", "30 minutos")
        val adapter = ArrayAdapter(requireContext(), android.R.layout.simple_list_item_1, options)
        binding.warningDropdown.setAdapter(adapter)

        if (editPosition != -1) {

            val originalTake = TakeRepository.getTakes()[editPosition]

            val take = originalTake.copy(
                id = if (originalTake.id.isBlank()) {
                    "take_" + System.currentTimeMillis()
                } else originalTake.id
            )

            binding.timeInput.setText(take.time)

            chips.forEach { chip ->
                chip.isChecked =
                    take.days.any { it.label == chip.text.toString() }
            }

            medicines.clear()
            medicines.addAll(take.medicines ?: emptyList())
            medicineAdapter.notifyDataSetChanged()

            binding.reminderSwitch.isChecked = take.reminderEnabled

            take.advanceWarningMinutes?.let {
                binding.warningDropdown.setText("$it minutos", false)
            }
        }
    }

    private fun saveTakesLocally() {

        val prefs = requireContext()
            .getSharedPreferences("app", Context.MODE_PRIVATE)

        val json = JsonUtils.takesToJson(TakeRepository.getTakes())

        prefs.edit()
            .putString("takes", json)
            .apply()
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}