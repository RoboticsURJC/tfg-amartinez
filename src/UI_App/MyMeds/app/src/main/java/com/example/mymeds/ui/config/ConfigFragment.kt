package com.example.mymeds.ui.config

import android.Manifest
import android.content.Context
import android.content.pm.PackageManager
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.core.content.ContextCompat
import androidx.fragment.app.Fragment
import androidx.navigation.fragment.findNavController
import com.example.mymeds.R
import com.example.mymeds.databinding.FragmentConfigPillsBinding
import com.example.mymeds.data.repository.EspConfig
import android.util.Log
import com.example.mymeds.data.network.EspApi

class ConfigFragment : Fragment() {

    private var _binding: FragmentConfigPillsBinding? = null
    private val binding get() = _binding!!

    private var deviceDetected = false

    companion object {
        private const val LOCATION_PERMISSION_REQUEST = 1001
    }

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {

        _binding = FragmentConfigPillsBinding.inflate(inflater, container, false)

        binding.buttonBack.setOnClickListener {
            findNavController().navigate(R.id.homeFragment)
        }

        return binding.root
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)
        checkLocationPermissionAndStart()
    }

    override fun onResume() {
        super.onResume()

        EspApi.syncPinFromEsp()
    }

    // ---------------- PERMISOS ----------------

    private fun checkLocationPermissionAndStart() {
        if (ContextCompat.checkSelfPermission(
                requireContext(),
                Manifest.permission.ACCESS_FINE_LOCATION
            ) == PackageManager.PERMISSION_GRANTED
        ) {
            startDeviceCheck()
        } else {
            requestPermissions(
                arrayOf(Manifest.permission.ACCESS_FINE_LOCATION),
                LOCATION_PERMISSION_REQUEST
            )
        }
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        if (requestCode == LOCATION_PERMISSION_REQUEST) {
            if (grantResults.isNotEmpty() &&
                grantResults[0] == PackageManager.PERMISSION_GRANTED
            ) {
                startDeviceCheck()
            } else {
                deviceDetected = false
                updateUI()
            }
        }
    }

    // ---------------- LÓGICA PRINCIPAL ----------------

    private fun startDeviceCheck() {

        val prefs = requireContext()
            .getSharedPreferences("app", Context.MODE_PRIVATE)

        val savedUrl = prefs.getString("esp_url", null)

        Thread {

            var success = false
            var finalUrl: String? = savedUrl

            // 🔹 1. Intentar con URL guardada
            if (savedUrl != null) {
                Log.d("CONFIG", "Probando URL guardada: $savedUrl")
                success = checkConnection(savedUrl)
            }

            // 🔥 2. SI FALLA → DISCOVERY UDP
            if (!success) {

                Log.d("CONFIG", "Fallback → buscando ESP por UDP")

                val foundIp = discoverEsp()

                if (foundIp != null) {

                    finalUrl = "http://$foundIp"

                    Log.d("CONFIG", "Nueva IP encontrada: $finalUrl")

                    success = checkConnection(finalUrl)

                    if (success) {
                        prefs.edit().putString("esp_url", finalUrl).apply()
                    }
                }
            }

            deviceDetected = success

            if (!success) {

                prefs.edit()
                    .remove("esp_url")
                    .apply()

                EspConfig.baseUrl = ""
            }

            if (success && finalUrl != null) {
                EspConfig.baseUrl = finalUrl
            }

            activity?.runOnUiThread {

                if (!isAdded || _binding == null) {
                    return@runOnUiThread
                }

                updateUI()
            }

        }.start()
    }

    // ---------------- CHECK HTTP ----------------

    private fun checkConnection(urlBase: String): Boolean {
        return try {

            val url = java.net.URL("$urlBase/ping")
            val conn = url.openConnection() as java.net.HttpURLConnection

            conn.requestMethod = "GET"
            conn.setRequestProperty("X-DEVICE-TOKEN", "mymeds_secure_token")
            conn.connectTimeout = 2000
            conn.readTimeout = 2000

            val code = conn.responseCode

            Log.d("CONFIG", "Check $urlBase → $code")

            if (code == 200) {

                val response =
                    conn.inputStream
                        .bufferedReader()
                        .readText()

                response == "MYMEDS_ESP_OK"

            } else {

                false
            }

        } catch (e: Exception) {
            Log.d("CONFIG", "Check falló: $urlBase", e)
            false
        }
    }

    // ---------------- DISCOVERY UDP ----------------

    private fun discoverEsp(): String? {

        return try {

            val socket = java.net.DatagramSocket()
            socket.broadcast = true
            socket.soTimeout = 2000

            val message = "DISCOVER_ESP".toByteArray()

            val wifiManager = requireContext().applicationContext
                .getSystemService(Context.WIFI_SERVICE) as android.net.wifi.WifiManager

            val ipInt = wifiManager.connectionInfo.ipAddress

            val ip = java.net.InetAddress.getByAddress(
                byteArrayOf(
                    (ipInt and 0xff).toByte(),
                    (ipInt shr 8 and 0xff).toByte(),
                    (ipInt shr 16 and 0xff).toByte(),
                    (ipInt shr 24 and 0xff).toByte()
                )
            )

            val ipString = ip.hostAddress ?: return null
            val parts = ipString.split(".")
            val broadcast = "${parts[0]}.${parts[1]}.${parts[2]}.255"

            val address = java.net.InetAddress.getByName(broadcast)

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

            if (received == "ESP_HERE") {
                val foundIp = response.address.hostAddress
                socket.close()
                return foundIp
            }

            socket.close()
            null

        } catch (e: Exception) {
            Log.d("CONFIG", "Discovery falló", e)
            null
        }
    }

    // ---------------- UI ----------------

    private fun updateUI() {

        if (_binding == null) {
            return
        }

        if (!deviceDetected) {

            binding.textStatus.text =
                "No hay ningún dispensador configurado"

            binding.buttonConf.text =
                "Escanear QR"

        } else {

            binding.textStatus.text =
                "Dispensador conectado"

            binding.buttonConf.text =
                "Gestionar tomas"
        }

        binding.buttonConf.visibility = View.VISIBLE

        binding.buttonConf.setOnClickListener {

            if (!deviceDetected) {

                val bundle = Bundle()

                bundle.putString(
                    "mode",
                    "WIFI"
                )

                findNavController().navigate(
                    R.id.qrScannerFragment,
                    bundle
                )

            } else {

                findNavController().navigate(
                    R.id.takesListFragment
                )
            }
        }
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}