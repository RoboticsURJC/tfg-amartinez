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
import com.example.mymeds.data.repository.TakeRepository

class TakesListFragment : Fragment() {
    private var _binding: FragmentTakesListBinding? = null
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        _binding = FragmentTakesListBinding.inflate(inflater, container, false)

        binding.buttonBack.setOnClickListener(){
            findNavController().popBackStack()
        }

        binding.buttonAddFirstTake.setOnClickListener(){
            findNavController().navigate(R.id.takesConfigFragment)
        }

        binding.buttonAddTake.setOnClickListener(){
            findNavController().navigate(R.id.takesConfigFragment)
        }

        binding.buttonEditTake.setOnClickListener(){
            findNavController().navigate(R.id.takesConfigFragment)
        }
        binding.buttonDeleteTake.setOnClickListener(){
            showDeleteConfirmation(0)
        }

        return binding.root
    }

    override fun onResume() {
        super.onResume()
        updateUI()
    }

    private fun updateUI(){
        val takes = TakeRepository.getTakes()

        if(takes.isEmpty()){
            binding.emptyState.visibility = View.VISIBLE
            binding.takesState.visibility = View.GONE
        } else {
            binding.emptyState.visibility = View.GONE
            binding.takesState.visibility = View.VISIBLE
        }
    }

    private fun showDeleteConfirmation(position: Int){
        AlertDialog.Builder(requireContext())
            .setTitle("Eliminar toma")
            .setMessage("¿Estás seguro de que quieres eliminar esta toma?\nEsta acción no se puede deshacer.")
            .setPositiveButton("Eliminar"){
                _,_->

                TakeRepository.removeTake(position)
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