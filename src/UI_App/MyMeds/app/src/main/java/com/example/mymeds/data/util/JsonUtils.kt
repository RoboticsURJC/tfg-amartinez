package com.example.mymeds.data.util

import com.example.mymeds.data.model.Take
import com.google.gson.Gson

object JsonUtils {

    private val gson = Gson()

    fun takeToJson(take: Take): String {
        return gson.toJson(take)
    }

    fun takesToJson(takes: List<Take>): String {
        return gson.toJson(takes)
    }
}