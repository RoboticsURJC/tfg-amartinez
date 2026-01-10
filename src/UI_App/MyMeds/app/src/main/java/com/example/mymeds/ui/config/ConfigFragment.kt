package com.example.mymeds.ui.config

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import androidx.navigation.fragment.findNavController
import com.example.mymeds.R
import com.example.mymeds.databinding.FragmentConfigPillsBinding

class ConfigFragment : Fragment() {

    private var _binding: FragmentConfigPillsBinding? = null
    private val binding get() = _binding!!

    // Simulación hasta usar ESP32 real
    private var deviceDetected = false

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {

        _binding = FragmentConfigPillsBinding.inflate(inflater, container, false)

        updateUI()

        binding.buttonConf.setOnClickListener {
            findNavController().navigate(R.id.qrScannerFragment)
        }

        binding.buttonBack.setOnClickListener {
            findNavController().popBackStack()
        }

        return binding.root
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }

    private fun updateUI() {
        if (!deviceDetected) {
            binding.textStatus.text = "No hay ningún dispensador configurado"
            binding.buttonConf.visibility = View.VISIBLE
        } else {
            binding.textStatus.text = "Dispensador detectado correctamente"
            binding.buttonConf.visibility = View.GONE
        }
    }
}
