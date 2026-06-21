package com.example.mymeds.repository

import android.content.Context
import com.example.mymeds.data.PulseMeasurement
import com.google.gson.Gson
import com.google.gson.reflect.TypeToken

object PulseStorage {

    private const val PREF_NAME = "pulse_history"
    private const val KEY_DATA = "measurements"

    fun save(
        context: Context,
        measurements: List<PulseMeasurement>
    ) {
        val prefs =
            context.getSharedPreferences(
                PREF_NAME,
                Context.MODE_PRIVATE
            )

        val json =
            Gson().toJson(measurements)

        prefs.edit()
            .putString(KEY_DATA, json)
            .apply()
    }

    fun load(
        context: Context
    ): List<PulseMeasurement> {

        val prefs =
            context.getSharedPreferences(
                PREF_NAME,
                Context.MODE_PRIVATE
            )

        val json =
            prefs.getString(KEY_DATA, null)
                ?: return emptyList()

        val type =
            object :
                TypeToken<List<PulseMeasurement>>() {}.type

        return Gson().fromJson(json, type)
    }
}