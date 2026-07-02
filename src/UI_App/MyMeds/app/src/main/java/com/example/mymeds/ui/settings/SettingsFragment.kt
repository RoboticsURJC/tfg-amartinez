package com.example.mymeds.ui.settings

import android.content.Context
import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Toast
import androidx.fragment.app.Fragment
import com.example.mymeds.databinding.FragmentSettingsBinding
import androidx.navigation.fragment.findNavController
import com.example.mymeds.R
import com.example.mymeds.data.repository.EspConfig
import com.example.mymeds.data.network.EspApi
import androidx.appcompat.app.AlertDialog
import com.example.mymeds.data.repository.MedicineRepository
import com.example.mymeds.data.repository.TakeRepository
import com.example.mymeds.data.util.JsonUtils
import com.example.mymeds.data.util.MedicineStorage
import com.example.mymeds.notifications.NotificationScheduler

class SettingsFragment : Fragment() {

    private var _binding: FragmentSettingsBinding? = null
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {

        _binding = FragmentSettingsBinding.inflate(
            inflater,
            container,
            false
        )

        binding.buttonChangePin.setOnClickListener {
            findNavController().navigate(R.id.navigation_change_pin)
        }

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

        if (EspConfig.baseUrl.isNotBlank()) {

            binding.textDeviceStatus.text = "🟢 Dispositivo configurado"
            binding.textDeviceIp.text = EspConfig.baseUrl

        } else {

            binding.textDeviceStatus.text = "🔴 No configurado"
            binding.textDeviceIp.text = ""
        }

        updateDeviceStatus()

        setupNotificationSettings()

        binding.buttonSync.setOnClickListener {
            synchronizeNow()
        }

        binding.buttonInfo.setOnClickListener {
            showInfoDialog()
        }

        binding.buttonReset.setOnClickListener {
            showResetDialog()
        }
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }

    private fun updateDeviceStatus() {

        if (EspConfig.baseUrl.isBlank()) {

            binding.textDeviceStatus.text = "🔴 Dispositivo no configurado"
            binding.textDeviceIp.text = ""

            return
        }

        binding.textDeviceStatus.text = "🟠 Comprobando conexión..."
        binding.textDeviceIp.text = EspConfig.baseUrl

        EspApi.checkConnection { connected ->

            requireActivity().runOnUiThread {

                if (_binding == null) return@runOnUiThread

                binding.textDeviceStatus.text =
                    if (connected)
                        "🟢 Dispositivo conectado"
                    else
                        "🔴 Dispositivo desconectado"
            }
        }
    }

    private fun setupNotificationSettings() {

        val prefs =
            requireContext().getSharedPreferences(
                "app",
                Context.MODE_PRIVATE
            )

        binding.switchNotifications.isChecked =
            prefs.getBoolean(
                "notifications_enabled",
                true
            )

        binding.switchNotifications.setOnCheckedChangeListener { _, checked ->

            prefs.edit()
                .putBoolean(
                    "notifications_enabled",
                    checked
                )
                .apply()
        }
    }

    private fun synchronizeNow() {

        binding.buttonSync.isEnabled = false
        binding.buttonSync.text = "Sincronizando..."

        EspApi.synchronizeAll(requireContext()) { success ->

            requireActivity().runOnUiThread {

                binding.buttonSync.isEnabled = true
                binding.buttonSync.text = "🔄 Sincronizar ahora"

                Toast.makeText(
                    requireContext(),
                    if (success)
                        "Sincronización completada"
                    else
                        "No se pudo conectar con el dispositivo",
                    Toast.LENGTH_SHORT
                ).show()
            }
        }
    }

    private fun showInfoDialog() {

        AlertDialog.Builder(requireContext())
            .setTitle("Acerca de MyMeds")
            .setMessage(
                """
            MyMeds v1.0
            
            Aplicación desarrollada como Trabajo Fin de Grado.
            
            Titulación:
            Ingeniería Robótica Software
            
            Universidad:
            Universidad Rey Juan Carlos
            
            Autora:
            Ana Martínez Albendea
            
            © 2026
            """.trimIndent()
            )
            .setPositiveButton("Aceptar", null)
            .show()
    }

    private fun showResetDialog() {

        AlertDialog.Builder(requireContext())
            .setTitle("Restablecer aplicación")
            .setMessage(
                "Se eliminarán todos los datos de la aplicación y será necesario volver a vincular el dispositivo.\n\n¿Desea continuar?"
            )
            .setPositiveButton("Restablecer") { _, _ ->
                resetApplication()
            }
            .setNegativeButton("Cancelar", null)
            .show()
    }

    private fun resetApplication() {

        NotificationScheduler.cancelAll(requireContext())

        MedicineRepository.clear()
        TakeRepository.clear()

        MedicineStorage.save(requireContext())

        val prefs =
            requireContext().getSharedPreferences(
                "app",
                Context.MODE_PRIVATE
            )

        prefs.edit().clear().apply()

        EspConfig.baseUrl = ""

        AlertDialog.Builder(requireContext())
            .setTitle("Aplicación restablecida")
            .setMessage(
                "La aplicación se ha restablecido a su estado inicial.\n\nSerá necesario volver a configurar el dispositivo para continuar."
            )
            .setPositiveButton("Aceptar") { _, _ ->

                findNavController().navigate(
                    R.id.homeFragment,
                    null,
                    androidx.navigation.NavOptions.Builder()
                        .setPopUpTo(R.id.homeFragment, true)
                        .build()
                )

            }
            .setCancelable(false)
            .show()
    }
}