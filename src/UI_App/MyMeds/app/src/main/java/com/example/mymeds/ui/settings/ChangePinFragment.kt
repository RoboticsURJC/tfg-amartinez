package com.example.mymeds.ui.settings

import android.content.Context
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.fragment.app.Fragment
import androidx.navigation.fragment.findNavController
import com.example.mymeds.data.network.EspApi
import com.example.mymeds.databinding.FragmentChangePinBinding

class ChangePinFragment : Fragment() {

    private var _binding: FragmentChangePinBinding? = null
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {

        _binding =
            FragmentChangePinBinding.inflate(
                inflater,
                container,
                false
            )

        return binding.root
    }

    override fun onViewCreated(
        view: View,
        savedInstanceState: Bundle?
    ) {
        super.onViewCreated(view, savedInstanceState)

        binding.buttonBack.setOnClickListener {
            findNavController().navigateUp()
        }

        binding.buttonSavePin.setOnClickListener {

            val currentPin =
                binding.currentPinInput.text.toString()

            val newPin =
                binding.newPinInput.text.toString()

            val confirmPin =
                binding.confirmPinInput.text.toString()

            val prefs =
                requireContext().getSharedPreferences(
                    "app",
                    Context.MODE_PRIVATE
                )

            val savedPin =
                prefs.getString(
                    "app_pin",
                    "1234"
                )

            if (currentPin != savedPin) {

                Toast.makeText(
                    requireContext(),
                    "PIN actual incorrecto",
                    Toast.LENGTH_SHORT
                ).show()

                return@setOnClickListener
            }

            if (newPin.length != 4) {

                Toast.makeText(
                    requireContext(),
                    "El PIN debe tener 4 dígitos",
                    Toast.LENGTH_SHORT
                ).show()

                return@setOnClickListener
            }

            if (newPin != confirmPin) {

                Toast.makeText(
                    requireContext(),
                    "Los PIN no coinciden",
                    Toast.LENGTH_SHORT
                ).show()

                return@setOnClickListener
            }

            prefs.edit()
                .putString(
                    "app_pin",
                    newPin
                )
                .apply()

            EspApi.sendPin(newPin)

            Toast.makeText(
                requireContext(),
                "PIN actualizado",
                Toast.LENGTH_SHORT
            ).show()

            findNavController().navigateUp()
        }
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}