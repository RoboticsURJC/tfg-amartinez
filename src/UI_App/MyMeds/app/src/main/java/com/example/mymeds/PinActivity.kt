package com.example.mymeds

import android.content.Context
import android.content.Intent
import android.os.Bundle
import android.util.Log
import android.widget.Toast
import androidx.appcompat.app.AppCompatActivity
import com.example.mymeds.databinding.ActivityPinBinding

class PinActivity : AppCompatActivity() {

    private lateinit var binding: ActivityPinBinding

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        binding = ActivityPinBinding.inflate(layoutInflater)

        setContentView(binding.root)

        binding.buttonUnlock.setOnClickListener {

            val enteredPin =
                binding.pinInput.text
                    ?.toString()
                    ?.trim()

            val prefs =
                getSharedPreferences(
                    "app",
                    Context.MODE_PRIVATE
                )

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

                Log.d(
                    "PIN_DEBUG",
                    "PIN correcto, abriendo MainActivity"
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