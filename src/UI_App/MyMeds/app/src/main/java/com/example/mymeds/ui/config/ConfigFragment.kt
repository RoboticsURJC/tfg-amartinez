package com.example.mymeds.ui.config

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.fragment.app.Fragment
import androidx.navigation.fragment.findNavController
import com.example.mymeds.R
import com.example.mymeds.databinding.FragmentConfigPillsBinding
import com.example.mymeds.data.repository.EspConfig
import android.content.Context

class ConfigFragment : Fragment() {

    private var _binding: FragmentConfigPillsBinding? = null
    private val binding get() = _binding!!

    private var deviceDetected = false

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {

        _binding = FragmentConfigPillsBinding.inflate(inflater, container, false)

        binding.buttonBack.setOnClickListener {
            findNavController().popBackStack()
        }

        val prefs = requireContext()
            .getSharedPreferences("app", Context.MODE_PRIVATE)

        val savedUrl = prefs.getString("esp_url", null)

        if (savedUrl != null) {

            EspConfig.baseUrl = savedUrl

            Thread {
                try {
                    val url = java.net.URL(savedUrl)
                    val conn = url.openConnection() as java.net.HttpURLConnection
                    conn.connectTimeout = 2000
                    conn.connect()

                    deviceDetected = conn.responseCode == 200

                } catch (e: Exception) {
                    deviceDetected = false
                }

                activity?.runOnUiThread {
                    updateUI()
                }

            }.start()

        } else {
            deviceDetected = false
            updateUI()
        }

        return binding.root
    }

    private fun updateUI() {

        if (!deviceDetected) {

            binding.textStatus.text = "No hay ningún dispensador configurado"
            binding.buttonConf.text = "Escanear QR"

        } else {

            binding.textStatus.text = "Dispensador conectado"
            binding.buttonConf.text = "Gestionar tomas"
        }

        binding.buttonConf.visibility = View.VISIBLE

        binding.buttonConf.setOnClickListener {

            if (!deviceDetected) {

                findNavController().navigate(R.id.qrScannerFragment)

            } else {

                findNavController().navigate(R.id.takesListFragment)
            }
        }
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}