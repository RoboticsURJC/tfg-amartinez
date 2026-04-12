package com.example.mymeds.ui.qr

import android.Manifest
import android.content.pm.PackageManager
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import androidx.navigation.fragment.findNavController
import androidx.core.content.ContextCompat
import androidx.activity.result.contract.ActivityResultContracts

import com.example.mymeds.R
import com.example.mymeds.databinding.FragmentQrScannerBinding
import com.example.mymeds.data.repository.EspConfig

import com.journeyapps.barcodescanner.BarcodeCallback
import com.journeyapps.barcodescanner.BarcodeResult

class QrScannerFragment : Fragment() {

    private var _binding: FragmentQrScannerBinding? = null
    private val binding get() = _binding!!

    private var scanned = false

    private val cameraPermissionLauncher =
        registerForActivityResult(ActivityResultContracts.RequestPermission()
        ) { granted ->
            if (granted) {
                startScanner()
            }
        }

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentQrScannerBinding.inflate(inflater, container, false)

        binding.buttonBack.setOnClickListener {
            findNavController().popBackStack()
        }

        binding.textInstruction.text = "Escanea el QR del dispensador"

        return binding.root
    }

    override fun onResume() {
        super.onResume()
        scanned = false

        if (
            ContextCompat.checkSelfPermission(
                requireContext(),
                Manifest.permission.CAMERA
            ) == PackageManager.PERMISSION_GRANTED
        ) {
            startScanner()
        } else {
            cameraPermissionLauncher.launch(Manifest.permission.CAMERA)
        }
    }

    private fun startScanner() {
        binding.barcodeScanner.resume()

        binding.barcodeScanner.decodeContinuous(
            object : BarcodeCallback {
                override fun barcodeResult(result: BarcodeResult?) {

                    val qr = result?.text ?: return

                    if (scanned) return

                    scanned = true

                    binding.barcodeScanner.pause()

                    EspConfig.baseUrl = qr

                    findNavController().navigate(
                        R.id.wifiConfigFragment
                    )
                }
            }
        )
    }

    override fun onPause() {
        super.onPause()
        binding.barcodeScanner.pause()
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}