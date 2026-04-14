package com.example.mymeds.data.network

import android.content.Context
import android.net.wifi.WifiManager
import android.util.Log
import com.example.mymeds.data.repository.EspConfig
import java.net.DatagramPacket
import java.net.DatagramSocket
import java.net.InetAddress

object EspDiscovery {

    fun discover(context: Context, onResult: (Boolean) -> Unit) {

        Thread {

            var lock: WifiManager.MulticastLock? = null

            try {
                val wifi = context.applicationContext
                    .getSystemService(Context.WIFI_SERVICE) as WifiManager

                lock = wifi.createMulticastLock("esp_lock")
                lock.acquire()

                val socket = DatagramSocket()
                socket.broadcast = true

                val message = "DISCOVER_ESP"
                val data = message.toByteArray()

                val packet = DatagramPacket(
                    data,
                    data.size,
                    InetAddress.getByName("255.255.255.255"),
                    8888
                )

                Log.d("ESP_DISCOVERY", "Enviando broadcast...")

                socket.send(packet)

                val buffer = ByteArray(1024)
                val response = DatagramPacket(buffer, buffer.size)

                socket.soTimeout = 3000

                socket.receive(response)

                val ip = response.address.hostAddress

                Log.d("ESP_DISCOVERY", "ESP encontrado en: $ip")

                EspConfig.baseUrl = "http://$ip"

                socket.close()

                onResult(true)

            } catch (e: Exception) {

                Log.e("ESP_DISCOVERY", "No encontrado", e)

                onResult(false)

            } finally {
                lock?.release()
            }

        }.start()
    }
}