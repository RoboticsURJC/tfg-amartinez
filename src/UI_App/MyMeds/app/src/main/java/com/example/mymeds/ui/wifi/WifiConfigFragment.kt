package com.example.mymeds.ui.wifi

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.fragment.app.Fragment
import androidx.appcompat.app.AppCompatActivity
import androidx.navigation.fragment.findNavController
import com.example.mymeds.databinding.FragmentWifiConfigBinding
import android.view.inputmethod.InputMethodManager
import android.content.Context
import com.example.mymeds.R
import com.example.mymeds.data.repository.EspConfig

class WifiConfigFragment : Fragment() {

    private var _binding: FragmentWifiConfigBinding? = null
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentWifiConfigBinding.inflate(inflater, container, false)

        binding.buttonBack.setOnClickListener {
            findNavController().popBackStack()
        }

        binding.buttonSendWifi.setOnClickListener {
            val ssid = binding.ssidInput.text.toString()
            val pass = binding.passwordInput.text.toString()

            if (ssid.isBlank() || pass.isBlank()) {

                Toast.makeText(
                    requireContext(),
                    "Introduce SSID y contraseña",
                    Toast.LENGTH_SHORT
                ).show()

                return@setOnClickListener
            }

            Thread {
                try {
                    if (EspConfig.baseUrl.isEmpty()) {
                        return@Thread
                    }
                    
                    val url = java.net.URL(EspConfig.baseUrl + "/save")
                    val conn = url.openConnection() as java.net.HttpURLConnection

                    conn.requestMethod = "POST"
                    conn.doOutput = true

                    val data = "ssid=" + ssid + "&password=" + pass
                    val out = conn.outputStream

                    out.write(data.toByteArray())
                    out.flush()
                    out.close()

                    conn.responseCode

                    requireActivity().runOnUiThread {

                        Toast.makeText(
                            requireContext(),
                            "WiFi enviado. Conectando con el dispositivo...",
                            Toast.LENGTH_LONG
                        ).show()

                        searchDeviceAndConnect()
                    }

                } catch (e: Exception) {
                    requireActivity().runOnUiThread {
                        Toast.makeText(
                            requireContext(),
                            "Error enviando WiFi",
                            Toast.LENGTH_LONG
                        ).show()
                    }
                }
            }.start()
        }

        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        Toast.makeText(
            requireContext(),
            EspConfig.baseUrl,
            Toast.LENGTH_LONG
        ).show()
        
        // Forzar foco y teclado en SSID
        binding.ssidInput.requestFocus()
        binding.ssidInput.post {
            val imm = requireContext()
                .getSystemService(Context.INPUT_METHOD_SERVICE)
                    as InputMethodManager
            imm.showSoftInput(
                binding.ssidInput,
                InputMethodManager.SHOW_IMPLICIT
            )
        }
    }

    private fun searchDeviceAndConnect() {

        Thread {
            var found = false

            for (i in 1..255) {
                try {
                    val ip = "http://192.168.1.$i"
                    val url = java.net.URL(ip + "/link")

                    val conn = url.openConnection() as java.net.HttpURLConnection
                    conn.connectTimeout = 200
                    conn.connect()

                    if (conn.responseCode == 200) {

                        found = true

                        activity?.runOnUiThread {

                            EspConfig.baseUrl = ip

                            val prefs = requireContext()
                                .getSharedPreferences("app", Context.MODE_PRIVATE)

                            prefs.edit()
                                .putString("esp_url", ip)
                                .apply()

                            Toast.makeText(
                                requireContext(),
                                "Dispositivo conectado",
                                Toast.LENGTH_SHORT
                            ).show()

                            findNavController().navigate(R.id.takesListFragment)
                        }

                        break
                    }

                } catch (_: Exception) {}
            }

            if (!found) {
                activity?.runOnUiThread {
                    Toast.makeText(
                        requireContext(),
                        "No se encontró el dispositivo",
                        Toast.LENGTH_LONG
                    ).show()
                }
            }

        }.start()
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
