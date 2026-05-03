package com.example.mymeds.data.model

data class Take(
    val id: String = "",
    val time: String,
    val days: List<DayOfWeek> = emptyList(),
    val medicines: List<TakeMedicine> = emptyList(),
    val reminderEnabled: Boolean = false,
    val advanceWarningMinutes: Int? = null
)
