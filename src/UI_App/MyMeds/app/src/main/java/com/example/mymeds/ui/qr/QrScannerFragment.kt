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

        return binding.root
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(
            requestCode,
            permissions,
            grantResults
        )

        if (requestCode == 100 &&
            grantResults.isNotEmpty() &&
            grantResults[0] == PackageManager.PERMISSION_GRANTED
        ) {

            binding.barcodeScanner.resume()
        }
    }

    override fun onResume() {
        super.onResume()

        if (
            ContextCompat.checkSelfPermission(
                requireContext(),
                Manifest.permission.CAMERA
            ) == PackageManager.PERMISSION_GRANTED
        ) {

            startScanner()

        } else {

            cameraPermissionLauncher.launch(
                Manifest.permission.CAMERA
            )
        }
    }

    private fun startScanner() {
        binding.barcodeScanner.resume()
        binding.barcodeScanner.decodeContinuous(
            object : BarcodeCallback {
                override fun barcodeResult(
                    result: BarcodeResult?
                ) {

                    result?.text?.let { qr ->
                        EspConfig.baseUrl = qr
                        binding.barcodeScanner.pause()
                        findNavController().navigate(R.id.wifiConfigFragment)
                    }
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
