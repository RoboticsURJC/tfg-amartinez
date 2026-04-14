package com.example.mymeds.data.util

import com.example.mymeds.data.model.Take
import com.google.gson.Gson

object JsonUtils {

    private val gson = Gson()
    private val takes = mutableListOf<Take>()

    fun takeToJson(take: Take): String {
        return gson.toJson(take)
    }

    fun takesToJson(takes: List<Take>): String {
        val map = mapOf("takes" to takes)
        return gson.toJson(map)
    }

    fun jsonToTakes(json: String): List<Take> {
        android.util.Log.d("JSON_RAW", json)
        return try {
            val wrapper = gson.fromJson(json, TakesWrapper::class.java)
            wrapper.takes
        } catch (e: Exception) {
            android.util.Log.e("JSON", "Error parsing takes", e)
            emptyList()
        }
    }

    data class TakesWrapper(
        val takes: List<Take> = emptyList()
    )
}