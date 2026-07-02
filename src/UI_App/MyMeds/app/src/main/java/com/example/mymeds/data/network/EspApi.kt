package com.example.mymeds.data.network

import android.util.Log
import com.example.mymeds.data.repository.EspConfig
import com.example.mymeds.data.repository.TakeRepository
import com.example.mymeds.data.util.JsonUtils
import com.example.mymeds.data.repository.MedicineRepository
import android.content.Context
import com.example.mymeds.MyMedsApplication
import com.example.mymeds.data.util.MedicineStorage

object EspApi {

    private fun getToken(): String {

        val prefs =
            MyMedsApplication.instance
                .getSharedPreferences(
                    "app",
                    Context.MODE_PRIVATE
                )

        val token =
            prefs.getString(
                "device_token",
                ""
            ) ?: ""

        Log.d(
            "TOKEN_DEBUG",
            "Token leído: '$token'"
        )

        return token
    }

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

                conn.setRequestProperty("X-DEVICE-TOKEN", getToken())

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

    fun getTakes(onResult: (String?) -> Unit) {

        if (EspConfig.baseUrl.isEmpty()) {
            onResult(null)
            return
        }

        Thread {
            try {

                val url = java.net.URL("${EspConfig.baseUrl}/takes")
                val conn = url.openConnection() as java.net.HttpURLConnection

                conn.requestMethod = "GET"
                conn.setRequestProperty("X-DEVICE-TOKEN", getToken())
                conn.connectTimeout = 2000
                conn.readTimeout = 2000

                val code = conn.responseCode

                if (code == 200) {
                    val response = conn.inputStream.bufferedReader().readText()
                    Log.d("ESP_GET", response)
                    onResult(response)
                } else {
                    onResult(null)
                }

            } catch (e: Exception) {
                Log.e("ESP_GET", "Error", e)
                onResult(null)
            }

        }.start()
    }

    fun sendMedicines() {

        Log.d("ESP_TEST", "SEND MEDICINES EJECUTADO")

        if (EspConfig.baseUrl.isEmpty()) {

            Log.e("ESP_TEST", "BASE URL VACÍA")

            return
        }

        val meds =
            com.example.mymeds.data.repository
                .MedicineRepository.getAll()

        Thread {

            try {

                val url = java.net.URL(
                    EspConfig.baseUrl + "/medicines"
                )

                val conn =
                    url.openConnection() as java.net.HttpURLConnection

                conn.requestMethod = "POST"
                conn.doOutput = true

                conn.setRequestProperty("Content-Type", "application/json")
                conn.setRequestProperty("X-DEVICE-TOKEN", getToken())

                val json =
                    com.google.gson.Gson().toJson(
                        mapOf("medicines" to meds)
                    )

                Log.d("ESP_MED", json)

                conn.outputStream.use {
                    it.write(json.toByteArray())
                }

                val code = conn.responseCode

                Log.d(
                    "ESP_MED",
                    "Medicines enviados: $code"
                )

            } catch (e: Exception) {

                Log.e("ESP_MED", "Error", e)
            }

        }.start()
    }

    fun getMedicines(onResult: (String?) -> Unit) {

        if (EspConfig.baseUrl.isEmpty()) {
            onResult(null)
            return
        }

        Thread {
            try {

                val url =
                    java.net.URL("${EspConfig.baseUrl}/medicines")

                val conn =
                    url.openConnection() as java.net.HttpURLConnection

                conn.requestMethod = "GET"
                conn.setRequestProperty("X-DEVICE-TOKEN", getToken())

                conn.connectTimeout = 2000
                conn.readTimeout = 2000

                val code = conn.responseCode

                if (code == 200) {

                    val response =
                        conn.inputStream
                            .bufferedReader()
                            .readText()

                    Log.d("ESP_MED_GET", response)

                    onResult(response)

                } else {
                    onResult(null)
                }

            } catch (e: Exception) {

                Log.e("ESP_MED_GET", "Error", e)

                onResult(null)
            }

        }.start()
    }

    fun sendPin(pin: String) {

        Log.d(
            "ESP_PIN",
            "sendPin llamado con PIN: $pin"
        )

        if (EspConfig.baseUrl.isEmpty()) {

            Log.e(
                "ESP_PIN",
                "baseUrl vacía"
            )

            return
        }

        Thread {

            try {

                Log.d(
                    "ESP_PIN",
                    "Enviando a ${EspConfig.baseUrl}/pin"
                )

                val url =
                    java.net.URL(
                        "${EspConfig.baseUrl}/pin"
                    )

                val conn =
                    url.openConnection()
                            as java.net.HttpURLConnection

                conn.requestMethod = "POST"
                conn.doOutput = true

                conn.setRequestProperty(
                    "Content-Type",
                    "text/plain"
                )

                val token = getToken()

                Log.d(
                    "ESP_PIN",
                    "Token enviado: '$token'"
                )

                conn.setRequestProperty(
                    "X-DEVICE-TOKEN",
                    token
                )

                Log.d(
                    "ESP_PIN",
                    conn.requestProperties.toString()
                )

                conn.outputStream.use {

                    it.write(
                        pin.toByteArray()
                    )
                }

                Log.d(
                    "ESP_PIN",
                    "Código: ${conn.responseCode}"
                )

            } catch (e: Exception) {

                Log.e(
                    "ESP_PIN",
                    "Error",
                    e
                )
            }

        }.start()
    }

    fun getPin(
        onResult: (String?) -> Unit
    )
    {
        if (EspConfig.baseUrl.isEmpty()) {
            onResult(null)
            return
        }

        Thread {

            try {

                val url =
                    java.net.URL(
                        "${EspConfig.baseUrl}/pin"
                    )

                val conn =
                    url.openConnection()
                            as java.net.HttpURLConnection

                conn.requestMethod = "GET"

                conn.setRequestProperty(
                    "X-DEVICE-TOKEN",
                    getToken()
                )

                val code =
                    conn.responseCode

                if (code == 200)
                {
                    val pin =
                        conn.inputStream
                            .bufferedReader()
                            .readText()

                    onResult(pin)
                }
                else
                {
                    onResult(null)
                }

            } catch (e: Exception)
            {
                onResult(null)
            }

        }.start()
    }

    fun syncPinFromEsp() {

        EspApi.getPin { espPin ->

            if (espPin == null)
                return@getPin

            val prefs =
                MyMedsApplication.instance
                    .getSharedPreferences(
                        "app",
                        Context.MODE_PRIVATE
                    )

            val localPin =
                prefs.getString(
                    "app_pin",
                    "1234"
                ) ?: "1234"

            if (espPin != localPin)
            {
                prefs.edit()
                    .putString(
                        "app_pin",
                        espPin
                    )
                    .apply()

                Log.d(
                    "PIN_SYNC",
                    "PIN actualizado desde ESP: $espPin"
                )
            }
        }
    }

    fun refreshLink(onSuccess: (() -> Unit)? = null) {

        if (EspConfig.baseUrl.isEmpty()) {
            return
        }

        Thread {

            try {

                val url =
                    java.net.URL(
                        "${EspConfig.baseUrl}/link"
                    )

                val conn =
                    url.openConnection()
                            as java.net.HttpURLConnection

                conn.requestMethod = "GET"

                if (conn.responseCode == 200) {

                    val response =
                        conn.inputStream
                            .bufferedReader()
                            .readText()

                    val json =
                        org.json.JSONObject(response)

                    val token =
                        json.getString("token")

                    val prefs =
                        MyMedsApplication.instance
                            .getSharedPreferences(
                                "app",
                                Context.MODE_PRIVATE
                            )

                    prefs.edit()
                        .putString(
                            "device_token",
                            token
                        )
                        .apply()

                    Log.d(
                        "LINK",
                        "Token actualizado: $token"
                    )

                    onSuccess?.invoke()
                }

            } catch (e: Exception) {

                Log.e(
                    "LINK",
                    "Error actualizando token",
                    e
                )
            }

        }.start()
    }

    fun getPulseHistory(onResult: (String?) -> Unit) {
        if (EspConfig.baseUrl.isEmpty()) {
            onResult(null)
            return
        }

        Thread {

            try {
                val url = java.net.URL("${EspConfig.baseUrl}/pulse-history")
                val conn = url.openConnection() as java.net.HttpURLConnection

                conn.requestMethod = "GET"
                conn.setRequestProperty(
                    "X-DEVICE-TOKEN",
                    getToken()
                )

                conn.connectTimeout = 2000
                conn.readTimeout = 2000

                if (conn.responseCode == 200) {
                    val response = conn.inputStream.bufferedReader().readText()
                    onResult(response)
                } else {
                    onResult(null)
                }

            } catch (e: Exception) {
                onResult(null)
            }

        }.start()
    }

    fun checkConnection(onResult: (Boolean) -> Unit) {

        getTakes { result ->

            onResult(result != null)
        }
    }

    fun synchronizeAll(
        context: Context,
        onResult: (Boolean) -> Unit
    ) {

        getMedicines { medicinesJson ->

            if (medicinesJson == null) {
                onResult(false)
                return@getMedicines
            }

            val medicines =
                JsonUtils.medicinesFromJson(
                    medicinesJson
                )

            MedicineRepository.setAll(
                medicines
            )

            MedicineStorage.save(
                context
            )

            getTakes { takesJson ->

                if (takesJson == null) {
                    onResult(false)
                    return@getTakes
                }

                val takes =
                    JsonUtils.jsonToTakes(
                        takesJson
                    )

                TakeRepository.clear()
                TakeRepository.addAll(
                    takes
                )

                val prefs =
                    context.getSharedPreferences(
                        "app",
                        Context.MODE_PRIVATE
                    )

                prefs.edit()
                    .putString(
                        "takes",
                        JsonUtils.takesToJson(takes)
                    )
                    .apply()

                onResult(true)
            }
        }
    }
}