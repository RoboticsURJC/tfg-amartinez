package com.example.mymeds.ui.takesList

import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.example.mymeds.R
import androidx.appcompat.app.AlertDialog
import androidx.navigation.fragment.findNavController
import com.example.mymeds.databinding.FragmentTakesListBinding

class TakesListFragment : Fragment() {
    private var _binding: FragmentTakesListBinding? = null
    private val binding get() = _binding!!

    //For simulation only
    private var hasTakesConfigured = true

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        _binding = FragmentTakesListBinding.inflate(inflater, container, false)

        updateUI()

        binding.buttonBack.setOnClickListener(){
            findNavController().popBackStack()
        }

        binding.buttonAddFirstTake.setOnClickListener(){
            //Later navigation
        }

        binding.buttonAddTake.setOnClickListener(){
            //Later navigation
        }

        binding.buttonEditTake.setOnClickListener(){
            //Later navigation
        }
        binding.buttonDeleteTake.setOnClickListener(){
            showDeleteConfirmation()
        }

        return binding.root
    }

    private fun updateUI(){
        if (hasTakesConfigured){
            binding.emptyState.visibility = View.GONE
            binding.takesState.visibility = View.VISIBLE
        } else{
            binding.emptyState.visibility = View.VISIBLE
            binding.takesState.visibility = View.GONE
        }
    }

    private fun showDeleteConfirmation(){
        AlertDialog.Builder(requireContext())
            .setTitle("Eliminar toma")
            .setMessage("¿Estás seguro de que quieres eliminar esta toma?\nEsta acción no se puede deshacer.")
            .setPositiveButton("Eliminar"){
                _,_->

                //For simulation
                hasTakesConfigured = false
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