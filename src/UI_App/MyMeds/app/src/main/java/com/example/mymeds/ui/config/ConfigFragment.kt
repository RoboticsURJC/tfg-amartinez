package com.example.mymeds.ui.config

import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.example.mymeds.R
import androidx.navigation.fragment.findNavController
import com.example.mymeds.databinding.FragmentConfigPillsBinding
import androidx.appcompat.app.AppCompatActivity

class ConfigFragment : Fragment() {

    private var _binding: FragmentConfigPillsBinding? = null
    private val binding get() = _binding!!

    // De momento para simular la conectividad hasta que pueda usar la ESP32
    private var deviceDetected = false

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentConfigPillsBinding.inflate(inflater, container, false)

        updateUI()

        binding.buttonConf.setOnClickListener{
            findNavController().navigate(R.id.qrScannerFragment)
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

    private fun updateUI(){
        if (!deviceDetected){
            binding.textStatus.text = "No hay ningún dispensador configurado"
            binding.buttonConf.visibility = View.VISIBLE
        } else{
            binding.textStatus.text = "Dispensador detectado correctamente"
            binding.buttonConf.visibility = View.GONE
        }
    }
}