package com.example.mymeds.ui.qr

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import androidx.navigation.fragment.findNavController
import com.example.mymeds.R
import com.example.mymeds.databinding.FragmentQrScannerBinding

class QrScannerFragment : Fragment() {

    private var _binding: FragmentQrScannerBinding? = null
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentQrScannerBinding.inflate(inflater, container, false)

        // Botón que SIMULA que el QR se ha leído correctamente
        binding.buttonSimulateQr.setOnClickListener {
            // En el futuro aquí llegará el ID real del dispositivo
            findNavController().navigate(R.id.wifiConfigFragment)
        }

        return binding.root
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}
