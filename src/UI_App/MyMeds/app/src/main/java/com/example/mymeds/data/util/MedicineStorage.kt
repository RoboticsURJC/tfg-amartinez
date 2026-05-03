package com.example.mymeds.data.util

import android.content.Context
import com.example.mymeds.data.model.Medicine
import com.example.mymeds.data.repository.MedicineRepository
import com.google.gson.Gson

object MedicineStorage {

    private const val PREF_NAME = "app"
    private const val KEY = "med_catalog"

    fun save(context: Context) {

        val prefs = context.getSharedPreferences(PREF_NAME, Context.MODE_PRIVATE)

        val json = Gson().toJson(MedicineRepository.getAll())

        prefs.edit()
            .putString(KEY, json)
            .apply()
    }

    fun load(context: Context) {

        val prefs = context.getSharedPreferences(PREF_NAME, Context.MODE_PRIVATE)

        val json = prefs.getString(KEY, null)

        if (json != null) {

            val list = Gson().fromJson(
                json,
                Array<Medicine>::class.java
            ).toList()

            MedicineRepository.setAll(list)
        }
    }
}