package com.example.mymeds.data.model

data class Take(
    val time: String,
    val days: List<DayOfWeek>,
    val medicines: List<Medicine>,
    val reminderEnabled: Boolean,
    val advanceWarningMinutes: Int?
)
