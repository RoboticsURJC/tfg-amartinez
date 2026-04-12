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

import android.content.Context

class QrScannerFragment : Fragment() {

    private var _binding: FragmentQrScannerBinding? = null
    private val binding get() = _binding!!

    private var scanned = false
    private var mode: String = "WIFI"

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

        mode = arguments?.getString("mode") ?: "WIFI"

        binding.textInstruction.text = when (mode) {
            "WIFI" -> "Escanea el QR para configurar el WiFi"
            "DEVICE" -> "Escanea el QR del dispositivo conectado"
            else -> ""
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
        scanned = false

        if (ContextCompat.checkSelfPermission(
                requireContext(),
                Manifest.permission.CAMERA
            ) == PackageManager.PERMISSION_GRANTED)
        {
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

                    if (!scanned) {
                        scanned = true
                        result?.text?.let { qr ->
                            binding.barcodeScanner.pause()

                            if (mode == "WIFI") {

                                EspConfig.baseUrl = qr

                                findNavController().navigate(
                                    R.id.wifiConfigFragment
                                )

                            } else {

                                EspConfig.baseUrl = qr

                                val prefs = requireContext()
                                    .getSharedPreferences("app", Context.MODE_PRIVATE)

                                prefs.edit()
                                    .putString("esp_url", qr)
                                    .apply()

                                Thread {
                                    try {
                                        val url = java.net.URL(qr + "/link")
                                        val conn = url.openConnection() as java.net.HttpURLConnection

                                        conn.requestMethod = "GET"
                                        conn.connect()

                                        conn.responseCode

                                        activity?.runOnUiThread {

                                            val prefs = requireContext()
                                                .getSharedPreferences("app", Context.MODE_PRIVATE)

                                            prefs.edit()
                                                .putString("esp_url", qr)
                                                .apply()

                                            findNavController().navigate(R.id.takesListFragment)
                                        }

                                    } catch (e: Exception) {
                                        e.printStackTrace()
                                    }
                                }.start()
                            }
                        }
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
