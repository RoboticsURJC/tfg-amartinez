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
import com.example.mymeds.data.repository.EspConfig
import com.example.mymeds.data.util.JsonUtils

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

        //if (EspConfig.baseUrl.isNotEmpty()) {

            //EspApi.getMedicines { json ->

                //if (json != null) {

                    //val meds =
                        //JsonUtils.medicinesFromJson(json)

                    //requireActivity().runOnUiThread {

                        //MedicineRepository.setAll(meds)

                        //MedicineStorage.save(requireContext())

                        //updateUI()
                    //}
                //}
            //}
        //}

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

        AlertDialog.Builder(requireContext())
            .setTitle("Nuevo medicamento")
            .setView(input)
            .setPositiveButton("Añadir") { _, _ ->

                val name = input.text.toString().trim()

                if (name.isBlank()) {
                    Toast.makeText(requireContext(), "Nombre vacío", Toast.LENGTH_SHORT).show()
                    return@setPositiveButton
                }

                val med = Medicine(
                    id = "med_" + System.currentTimeMillis(),
                    name = name
                )

                MedicineRepository.add(med)
                MedicineStorage.save(requireContext())

                android.util.Log.d("ESP_TEST", "VOY A ENVIAR MEDICINES")
                EspApi.sendMedicines()

                updateUI()
            }
            .setNegativeButton("Cancelar", null)
            .show()
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}