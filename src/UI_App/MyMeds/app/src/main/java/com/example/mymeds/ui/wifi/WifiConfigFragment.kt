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

    private var isSearching = false

    private var _binding: FragmentWifiConfigBinding? = null
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentWifiConfigBinding.inflate(inflater, container, false)

        binding.buttonBack.setOnClickListener {
            findNavController().navigate(R.id.configFragment)
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

            binding.buttonSendWifi.isEnabled = false

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

                        showLoading()
                        startConnectionProcess()
                    }

                } catch (e: Exception) {
                    requireActivity().runOnUiThread {
                        Toast.makeText(
                            requireContext(),
                            "Error enviando WiFi",
                            Toast.LENGTH_LONG
                        ).show()

                        binding.buttonSendWifi.isEnabled = true
                        showForm()
                    }
                }
            }.start()
        }

        return binding.root
    }

    private fun showLoading() {
        binding.layoutForm.visibility = View.GONE
        binding.layoutLoading.visibility = View.VISIBLE
    }

    private fun showForm() {
        binding.layoutForm.visibility = View.VISIBLE
        binding.layoutLoading.visibility = View.GONE
    }

    private fun startConnectionProcess() {

        if (isSearching) return
        isSearching = true

        Thread {

            try {

                Thread.sleep(4000)

                var foundIp: String? = null
                val lock = Object()

                for (attempt in 1..3) {

                    android.util.Log.d("DISCOVERY", "Intento $attempt")

                    for (i in 1..255 step 10) {

                        val threads = mutableListOf<Thread>()

                        for (j in i until i + 10) {

                            if (j > 255) break

                            val thread = Thread {

                                val ip = "http://192.168.1.$j"

                                try {
                                    val url = java.net.URL("$ip/link")
                                    val conn = url.openConnection() as java.net.HttpURLConnection

                                    conn.requestMethod = "GET"
                                    conn.connectTimeout = 500
                                    conn.readTimeout = 500

                                    val code = conn.responseCode

                                    if (code == 200) {
                                        synchronized(lock) {
                                            if (foundIp == null) {
                                                foundIp = ip
                                                android.util.Log.d("DISCOVERY", "✅ ENCONTRADO: $ip")
                                            }
                                        }
                                    }

                                } catch (_: Exception) {}
                            }

                            thread.start()
                            threads.add(thread)
                        }

                        threads.forEach { it.join() }

                        if (foundIp != null) break
                    }

                    if (foundIp != null) break

                    Thread.sleep(1000)
                }

                activity?.runOnUiThread {

                    isSearching = false
                    binding.buttonSendWifi.isEnabled = true

                    foundIp?.let { ip ->

                        EspConfig.baseUrl = ip

                        val prefs = requireContext()
                            .getSharedPreferences("app", Context.MODE_PRIVATE)

                        prefs.edit()
                            .putString("esp_url", ip)
                            .apply()

                        Toast.makeText(
                            requireContext(),
                            "Dispositivo conectado ($ip)",
                            Toast.LENGTH_SHORT
                        ).show()

                        findNavController().navigate(R.id.takesListFragment)

                    } ?: run {

                        Toast.makeText(
                            requireContext(),
                            "No se encontró el dispositivo",
                            Toast.LENGTH_LONG
                        ).show()

                        showForm()
                    }
                }

            } catch (e: Exception) {
                e.printStackTrace()
                isSearching = false
            }

        }.start()
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        Toast.makeText(
            requireContext(),
            EspConfig.baseUrl,
            Toast.LENGTH_LONG
        ).show()

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

    override fun onResume() {
        super.onResume()
        (requireActivity() as AppCompatActivity).supportActionBar?.show()
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}