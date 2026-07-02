package com.example.mymeds.ui.medicine

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.appcompat.app.AlertDialog
import androidx.fragment.app.Fragment
import androidx.navigation.fragment.findNavController
import androidx.recyclerview.widget.LinearLayoutManager
import com.example.mymeds.data.model.Medicine
import com.example.mymeds.data.repository.MedicineRepository
import com.example.mymeds.data.util.MedicineStorage
import com.example.mymeds.databinding.FragmentMedicineListBinding
import com.example.mymeds.R
import com.example.mymeds.data.network.EspApi

class MedicineListFragment : Fragment() {

    private var _binding: FragmentMedicineListBinding? = null
    private val binding get() = _binding!!

    private lateinit var adapter: MedicineListAdapter

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentMedicineListBinding.inflate(inflater, container, false)

        binding.buttonBack.setOnClickListener {
            findNavController().navigate(R.id.homeFragment)
        }

        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        setupRecycler()

        binding.buttonAddMedicine.setOnClickListener {
            showAddDialog()
        }

        updateUI()
    }

    private fun setupRecycler() {

        adapter = MedicineListAdapter(
            MedicineRepository.getAll(),
            onDelete = { position ->
                MedicineRepository.removeAt(position)
                MedicineStorage.save(requireContext())

                android.util.Log.d("ESP_TEST", "VOY A ENVIAR MEDICINES")
                EspApi.sendMedicines()
                updateUI()
            }
        )

        binding.recyclerMedicines.layoutManager = LinearLayoutManager(requireContext())
        binding.recyclerMedicines.adapter = adapter
    }

    private fun updateUI() {
        adapter.notifyDataSetChanged()
    }

    private fun showAddDialog() {

        val input = android.widget.EditText(requireContext())
        input.hint = "Nombre del medicamento"

        val dialog = AlertDialog.Builder(requireContext())
            .setTitle("Nuevo medicamento")
            .setView(input)
            .setPositiveButton("Añadir", null)
            .setNegativeButton("Cancelar", null)
            .create()

        dialog.setOnShowListener {

            dialog.getButton(AlertDialog.BUTTON_POSITIVE).setOnClickListener {

                val name = input.text.toString().trim()

                if (name.isBlank()) {
                    Toast.makeText(
                        requireContext(),
                        "Nombre vacío",
                        Toast.LENGTH_SHORT
                    ).show()
                    return@setOnClickListener
                }

                input.clearFocus()

                val imm =
                    requireContext().getSystemService(
                        android.content.Context.INPUT_METHOD_SERVICE
                    ) as android.view.inputmethod.InputMethodManager

                imm.hideSoftInputFromWindow(
                    input.windowToken,
                    0
                )

                val med = Medicine(
                    id = "med_" + System.currentTimeMillis(),
                    name = name
                )

                MedicineRepository.add(med)
                MedicineStorage.save(requireContext())

                android.util.Log.d("ESP_TEST", "VOY A ENVIAR MEDICINES")
                EspApi.sendMedicines()

                updateUI()

                dialog.dismiss()
            }
        }

        dialog.show()
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}