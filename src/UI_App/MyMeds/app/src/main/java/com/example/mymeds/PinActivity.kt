package com.example.mymeds

import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.util.Log
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.example.mymeds.data.network.EspApi
import com.example.mymeds.data.repository.EspConfig
import com.example.mymeds.databinding.ActivityPinBinding

class PinActivity : AppCompatActivity() {

    private lateinit var binding: ActivityPinBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityPinBinding.inflate(layoutInflater)

        setContentView(binding.root)

        // Desactivar mientras sincronizamos
        //binding.buttonUnlock.isEnabled = false

        val prefs =
            getSharedPreferences(
                "app",
                Context.MODE_PRIVATE
            )

        EspConfig.baseUrl =
            prefs.getString(
                "esp_url",
                ""
            ) ?: ""

        EspApi.getPin { espPin ->

            if (espPin != null) {

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

            runOnUiThread {
                binding.buttonUnlock.isEnabled = true
            }
        }

        binding.buttonUnlock.setOnClickListener {

            val enteredPin =
                binding.pinInput.text
                    ?.toString()
                    ?.trim()

            val savedPin =
                prefs.getString(
                    "app_pin",
                    "1234"
                )

            if (enteredPin == savedPin) {

                binding.pinInput.clearFocus()

                val imm =
                    getSystemService(
                        INPUT_METHOD_SERVICE
                    ) as android.view.inputmethod.InputMethodManager

                imm.hideSoftInputFromWindow(
                    binding.pinInput.windowToken,
                    0
                )

                val intent =
                    Intent(
                        this,
                        MainActivity::class.java
                    )

                intent.putExtra(
                    "AUTH_OK",
                    true
                )

                startActivity(intent)

                finish()

            } else {

                Toast.makeText(
                    this,
                    "PIN incorrecto",
                    Toast.LENGTH_SHORT
                ).show()
            }
        }
    }
}