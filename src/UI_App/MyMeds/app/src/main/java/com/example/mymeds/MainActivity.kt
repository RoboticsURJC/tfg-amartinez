package com.example.mymeds

import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.util.Log

import androidx.appcompat.app.AppCompatActivity
import androidx.core.splashscreen.SplashScreen.Companion.installSplashScreen

import com.example.mymeds.databinding.ActivityMainBinding

import com.example.mymeds.data.network.EspApi
import com.example.mymeds.data.repository.EspConfig
import com.example.mymeds.data.repository.MedicineRepository

import com.example.mymeds.data.util.JsonUtils
import com.example.mymeds.data.util.MedicineStorage

class MainActivity : AppCompatActivity() {

    private lateinit var binding: ActivityMainBinding
    private var keepSplash = true

    override fun onCreate(savedInstanceState: Bundle?) {

        Log.d(
            "MAIN_DEBUG",
            "MainActivity creada"
        )

        val splashScreen = installSplashScreen()

        splashScreen.setKeepOnScreenCondition {
            keepSplash
        }

        Handler(Looper.getMainLooper()).postDelayed({
            keepSplash = false
        }, 1500)

        super.onCreate(savedInstanceState)

        if (!intent.getBooleanExtra("AUTH_OK", false)) {

            startActivity(
                Intent(
                    this,
                    PinActivity::class.java
                )
            )

            finish()

            return
        }

        val prefs =
            getSharedPreferences(
                "app",
                Context.MODE_PRIVATE
            )

        // Cargar última URL conocida
        EspConfig.baseUrl =
            prefs.getString(
                "esp_url",
                ""
            ) ?: ""

        Log.d(
            "ESP_URL",
            "URL guardada: ${EspConfig.baseUrl}"
        )

        MedicineStorage.load(this)

        // Intentar redescubrir la ESP
        connectToEsp()

        binding =
            ActivityMainBinding.inflate(
                layoutInflater
            )

        setContentView(binding.root)
    }

    private fun connectToEsp() {

        Log.d(
            "ESP_CONNECT",
            "connectToEsp iniciado"
        )

        Thread {

            try {

                val socket = java.net.DatagramSocket()

                socket.broadcast = true

                val data = "DISCOVER_ESP".toByteArray()

                val packet = java.net.DatagramPacket(
                    data,
                    data.size,
                    java.net.InetAddress.getByName("255.255.255.255"),
                    8888
                )

                socket.send(packet)

                val buffer = ByteArray(256)

                val responsePacket = java.net.DatagramPacket(
                    buffer,
                    buffer.size
                )

                socket.soTimeout = 3000

                socket.receive(responsePacket)

                val message = String(
                    responsePacket.data,
                    0,
                    responsePacket.length
                )

                if (message == "ESP_HERE") {

                    val ip = responsePacket.address.hostAddress

                    Log.d(
                        "ESP_CONNECT",
                        "IP encontrada: $ip"
                    )

                    EspConfig.baseUrl = "http://$ip"

                    Log.d(
                        "ESP_CONNECT",
                        "URL actualizada: ${EspConfig.baseUrl}"
                    )

                    Log.d(
                        "ESP_CONNECT",
                        "Conectado a ESP: ${EspConfig.baseUrl}"
                    )

                    val prefs = getSharedPreferences(
                        "app",
                        Context.MODE_PRIVATE
                    )

                    prefs.edit()
                        .putString("esp_url", EspConfig.baseUrl)
                        .apply()

                    EspApi.refreshLink {

                        val currentPin =
                            prefs.getString(
                                "app_pin",
                                "1234"
                            ) ?: "1234"

                        Log.d(
                            "ESP_PIN",
                            "Sincronizando PIN: $currentPin"
                        )

                        EspApi.sendPin(currentPin)

                        EspApi.getMedicines { json ->

                            if (json != null) {

                                val meds =
                                    JsonUtils.medicinesFromJson(json)

                                runOnUiThread {

                                    MedicineRepository.setAll(meds)

                                    MedicineStorage.save(this)

                                    Log.d(
                                        "ESP_MED_SYNC",
                                        "Medicamentos sincronizados: ${meds.size}"
                                    )
                                }
                            }
                        }
                    }
                }

                socket.close()

            } catch (e: Exception) {

                Log.e(
                    "ESP_CONNECT",
                    "Error conectando",
                    e
                )
            }

        }.start()
    }
}