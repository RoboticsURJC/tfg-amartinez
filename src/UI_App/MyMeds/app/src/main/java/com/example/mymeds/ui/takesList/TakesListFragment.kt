package com.example.mymeds.ui.takesList

import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.example.mymeds.R
import androidx.appcompat.app.AlertDialog
import androidx.navigation.fragment.findNavController
import androidx.recyclerview.widget.LinearLayoutManager
import com.example.mymeds.databinding.FragmentTakesListBinding
import com.example.mymeds.data.repository.TakeRepository

class TakesListFragment : Fragment() {
    private var _binding: FragmentTakesListBinding? = null
    private val binding get() = _binding!!
    private lateinit var adapter: TakesAdapter

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

        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        val takes = TakeRepository.getTakes()

        adapter = TakesAdapter(takes,
            onEditClick = {position ->
                val bundle = Bundle()
                bundle.putInt("position", position)

                findNavController().navigate(
                    R.id.takesConfigFragment,
                    bundle)},
            onDeleteClick = {position -> showDeleteConfirmation(position)})

        binding.takesRecycler.adapter = adapter
        binding.takesRecycler.layoutManager = LinearLayoutManager(requireContext())
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

            adapter.notifyDataSetChanged()
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