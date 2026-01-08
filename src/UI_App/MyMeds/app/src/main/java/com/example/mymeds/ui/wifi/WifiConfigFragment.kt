package com.example.mymeds.ui.wifi

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.fragment.app.Fragment
import androidx.appcompat.app.AppCompatActivity
import com.example.mymeds.databinding.FragmentWifiConfigBinding

class WifiConfigFragment : Fragment() {

    private var _binding: FragmentWifiConfigBinding? = null
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentWifiConfigBinding.inflate(inflater, container, false)

        binding.buttonSendWifi.setOnClickListener {
            Toast.makeText(
                requireContext(),
                "Configuración WiFi enviada (simulado)",
                Toast.LENGTH_SHORT
            ).show()
        }

        return binding.root
    }

    override fun onResume() {
        super.onResume()
        (requireActivity() as AppCompatActivity).supportActionBar?.show()
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}
