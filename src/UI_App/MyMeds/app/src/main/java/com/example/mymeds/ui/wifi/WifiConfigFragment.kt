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
import com.example.mymeds.data.network.EspApi

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
                    if (EspConfig.baseUrl.isEmpty()) return@Thread

                    val url = java.net.URL(EspConfig.baseUrl + "/save")
                    val conn = url.openConnection() as java.net.HttpURLConnection

                    conn.requestMethod = "POST"
                    conn.doOutput = true

                    val data = "ssid=$ssid&password=$pass"

                    conn.outputStream.use {
                        it.write(data.toByteArray())
                    }

                    conn.responseCode

                    requireActivity().runOnUiThread {

                        Toast.makeText(
                            requireContext(),
                            "WiFi enviado. Conectando...",
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

    // ---------------- UI ----------------

    private fun showLoading() {
        binding.layoutForm.visibility = View.GONE
        binding.layoutLoading.visibility = View.VISIBLE
    }

    private fun showForm() {
        binding.layoutForm.visibility = View.VISIBLE
        binding.layoutLoading.visibility = View.GONE
    }

    // ---------------- RECONNECT ----------------

    private fun tryReconnectSavedDevice(): Boolean {

        val prefs = requireContext()
            .getSharedPreferences("app", Context.MODE_PRIVATE)

        val savedUrl = prefs.getString("esp_url", null) ?: return false

        EspConfig.baseUrl = savedUrl

        return try {
            val url = java.net.URL("$savedUrl/takes")
            val conn = url.openConnection() as java.net.HttpURLConnection

            conn.requestMethod = "GET"
            conn.connectTimeout = 1500
            conn.readTimeout = 1500

            val code = conn.responseCode

            if (code == 200) {

                // 🔥 CONFIRMAR LINK
                try {
                    val linkUrl = java.net.URL("$savedUrl/link")
                    val linkConn = linkUrl.openConnection() as java.net.HttpURLConnection

                    linkConn.requestMethod = "GET"
                    linkConn.connectTimeout = 1500
                    linkConn.readTimeout = 1500

                    linkConn.responseCode

                } catch (e: Exception) {
                    android.util.Log.d("RECONNECT", "Error en /link")
                }

                true

            } else false

        } catch (e: Exception) {
            false
        }
    }

    // ---------------- DISCOVERY ----------------

    private fun startConnectionProcess() {

        if (isSearching) return
        isSearching = true

        Thread {
            try {

                val socket = java.net.DatagramSocket()
                socket.broadcast = true
                socket.soTimeout = 2000

                val message = "DISCOVER_ESP".toByteArray()

                var foundIp: String? = null

                for (attempt in 1..5) {

                    val wifiManager = requireContext().applicationContext
                        .getSystemService(Context.WIFI_SERVICE) as android.net.wifi.WifiManager

                    val ipInt = wifiManager.connectionInfo.ipAddress

                    if (ipInt == 0) {
                        Thread.sleep(1500)
                        continue
                    }

                    val ip = java.net.InetAddress.getByAddress(
                        byteArrayOf(
                            (ipInt and 0xff).toByte(),
                            (ipInt shr 8 and 0xff).toByte(),
                            (ipInt shr 16 and 0xff).toByte(),
                            (ipInt shr 24 and 0xff).toByte()
                        )
                    )

                    val ipString = ip.hostAddress ?: continue
                    val parts = ipString.split(".")
                    val localBroadcast = "${parts[0]}.${parts[1]}.${parts[2]}.255"

                    val targets = listOf("255.255.255.255", localBroadcast)

                    for (target in targets) {
                        try {
                            val address = java.net.InetAddress.getByName(target)

                            val packet = java.net.DatagramPacket(
                                message,
                                message.size,
                                address,
                                8888
                            )

                            socket.send(packet)

                            val buffer = ByteArray(1024)
                            val response = java.net.DatagramPacket(buffer, buffer.size)

                            socket.receive(response)

                            val received = String(response.data, 0, response.length)
                            val senderIp = response.address.hostAddress

                            if (received == "ESP_HERE") {
                                foundIp = senderIp
                                break
                            }

                        } catch (_: Exception) {}
                    }

                    if (foundIp != null) break
                    Thread.sleep(2000)
                }

                socket.close()

                activity?.runOnUiThread {

                    isSearching = false
                    binding.buttonSendWifi.isEnabled = true

                    foundIp?.let { ip ->

                        val url = "http://$ip"
                        EspConfig.baseUrl = url

                        val prefs = requireContext()
                            .getSharedPreferences("app", Context.MODE_PRIVATE)

                        prefs.edit().putString("esp_url", url).apply()

                        // 🔥 LINK + ENVÍO TAKES
                        Thread {
                            try {
                                val linkUrl = java.net.URL("$url/link")
                                val conn = linkUrl.openConnection() as java.net.HttpURLConnection
                                conn.requestMethod = "GET"
                                conn.responseCode

                                Thread.sleep(300)
                                EspApi.sendTakes()

                            } catch (_: Exception) {}
                        }.start()

                        Toast.makeText(
                            requireContext(),
                            "Dispositivo conectado ($url)",
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
                activity?.runOnUiThread {
                    isSearching = false
                    binding.buttonSendWifi.isEnabled = true

                    Toast.makeText(
                        requireContext(),
                        "Error en búsqueda",
                        Toast.LENGTH_LONG
                    ).show()

                    showForm()
                }
            }
        }.start()
    }

    // ---------------- LIFECYCLE ----------------

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        showLoading()

        Thread {

            val success = tryReconnectSavedDevice()

            activity?.runOnUiThread {

                if (success) {

                    Toast.makeText(
                        requireContext(),
                        "Reconectado automáticamente",
                        Toast.LENGTH_SHORT
                    ).show()

                    findNavController().navigate(R.id.takesListFragment)

                } else {
                    showForm()

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