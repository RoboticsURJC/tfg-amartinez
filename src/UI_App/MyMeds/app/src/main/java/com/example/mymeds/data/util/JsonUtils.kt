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
        val type = object : com.google.gson.reflect.TypeToken<List<Take>>() {}.type
        val map: Map<String, List<Take>> = gson.fromJson(json, type)

        return map["takes"] ?: emptyList()
    }
}