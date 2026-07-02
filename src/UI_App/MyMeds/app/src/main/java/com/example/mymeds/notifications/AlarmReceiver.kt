package com.example.mymeds.notifications

import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import com.example.mymeds.data.repository.TakeRepository
import com.example.mymeds.data.repository.MedicineRepository
import com.example.mymeds.data.util.JsonUtils
import com.example.mymeds.data.util.MedicineStorage

class AlarmReceiver : BroadcastReceiver() {

    override fun onReceive(
        context: Context,
        intent: Intent
    ) {
        loadRepositories(context)

        val title = "MyMeds"

        val takeIndex =
            intent.getIntExtra(
                "takeIndex",
                -1
            )

        val isReminder =
            intent.getBooleanExtra(
                "isReminder",
                false
            )

        if (takeIndex !in TakeRepository.getTakes().indices) {
            android.util.Log.d("NOTIFICATIONS", "2 - índice inválido")
            return
        }

        val take = TakeRepository.getTakes()[takeIndex]

        val medicinesText =
            take.medicines.joinToString("\n") { takeMedicine ->

                val medicine =
                    MedicineRepository
                        .getAll()
                        .find {
                            it.id == takeMedicine.id
                        }

                val name =
                    medicine?.name
                        ?: "Medicamento"

                "• $name - ${takeMedicine.quantity} comprimido(s)"
            }

        val minutes = take.advanceWarningMinutes ?: 0

        val message =
            if (isReminder) {

                "En $minutes minutos deberá tomar:\n\n$medicinesText"

            } else {

                "Es la hora de tomar:\n\n$medicinesText"
            }

        val notificationId =
            intent.getIntExtra(
                "notificationId",
                0
            )

        NotificationHelper.showNotification(
            context = context,
            id = notificationId,
            title = title,
            text = message
        )
    }

    private fun loadRepositories(context: Context) {

        val prefs =
            context.getSharedPreferences(
                "app",
                Context.MODE_PRIVATE
            )

        prefs.getString("takes", null)?.let { json ->

            TakeRepository.clear()

            TakeRepository.addAll(
                JsonUtils.jsonToTakes(json)
            )
        }

        MedicineStorage.load(context)
    }
}