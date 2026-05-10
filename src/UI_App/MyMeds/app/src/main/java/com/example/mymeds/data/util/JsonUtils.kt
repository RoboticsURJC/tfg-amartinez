package com.example.mymeds.data.util

import com.example.mymeds.data.model.Take
import com.google.gson.Gson
import com.example.mymeds.data.model.Medicine
import org.json.JSONObject

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

            wrapper.takes.map { take ->

                val safeId = if (take.id.isBlank()) {
                    "take_" + System.currentTimeMillis()
                } else take.id

                take.copy(
                    id = safeId,
                    medicines = take.medicines ?: emptyList(),
                    days = take.days ?: emptyList()
                )
            }

        } catch (e: Exception) {
            android.util.Log.e("JSON", "Error parsing takes", e)
            emptyList()
        }
    }

    data class TakesWrapper(
        val takes: List<Take> = emptyList()
    )

    fun medicinesFromJson(json: String): List<Medicine> {

        return try {

            val obj = JSONObject(json)

            val arr = obj.getJSONArray("medicines")

            val result = mutableListOf<Medicine>()

            for (i in 0 until arr.length()) {

                val med = arr.getJSONObject(i)

                result.add(
                    Medicine(
                        id = med.getString("id"),
                        name = med.getString("name")
                    )
                )
            }

            result

        } catch (e: Exception) {

            emptyList()
        }
    }
}