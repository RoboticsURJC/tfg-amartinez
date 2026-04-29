package com.example.mymeds.data.network

import android.util.Log
import com.example.mymeds.data.repository.EspConfig
import com.example.mymeds.data.repository.TakeRepository
import com.example.mymeds.data.util.JsonUtils

object EspApi {

    fun sendTakes() {

        if (EspConfig.baseUrl.isEmpty()) return

        val takes = TakeRepository.getTakes()

        Thread {
            try {

                val url = java.net.URL(EspConfig.baseUrl + "/takes")

                val conn = url.openConnection() as java.net.HttpURLConnection
                conn.requestMethod = "POST"
                conn.doOutput = true
                conn.setRequestProperty("Content-Type", "application/json")

                val json = JsonUtils.takesToJson(takes)

                conn.outputStream.use {
                    it.write(json.toByteArray())
                }

                val code = conn.responseCode

                Log.d("ESP_SEND", "Takes enviados: $code")

            } catch (e: Exception) {
                Log.e("ESP_SEND", "Error", e)
            }
        }.start()
    }
}