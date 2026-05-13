package com.example.mymeds.ui.home

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.ImageView
import androidx.fragment.app.Fragment
import androidx.navigation.fragment.findNavController
import com.example.mymeds.databinding.FragmentHomeBinding
import androidx.appcompat.app.AppCompatActivity
import com.example.mymeds.R

class HomeFragment : Fragment() {

    private var _binding: FragmentHomeBinding? = null

    // This property is only valid between onCreateView and
    // onDestroyView.
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentHomeBinding.inflate(inflater, container, false)
        val root: View = binding.root

        // Configuración del botón con id button_config
        binding.buttonConf.setOnClickListener {
            findNavController().navigate(R.id.configFragment)
        }

        // Configuración del botón con id button_today_takes
        binding.buttonTodayTakes.setOnClickListener {
            findNavController().navigate(R.id.todayTakesFragment)
        }

        // Configuración del botón con id button_pulse_oxigen
        binding.buttonPulse.setOnClickListener {
            findNavController().navigate(R.id.pulse_oxigenFragment)
        }

        // Configuración del botón con id button_medicine
        binding.buttonMedicine.setOnClickListener {
            findNavController().navigate(R.id.medicineListFragment)
        }

        return binding.root
    }

    override fun onResume() {
        super.onResume()
        (requireActivity() as AppCompatActivity)
            .supportActionBar?.show()
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}