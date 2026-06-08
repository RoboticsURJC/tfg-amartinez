package com.example.mymeds.repository

import com.example.mymeds.data.PulseMeasurement

object PulseHistoryRepository {

    private val measurements =
        mutableListOf<PulseMeasurement>()

    fun setMeasurements(
        newMeasurements: List<PulseMeasurement>
    ) {
        measurements.clear()
        measurements.addAll(newMeasurements)
    }

    fun getMeasurements():
            List<PulseMeasurement>
    {
        return measurements
    }

    fun clear()
    {
        measurements.clear()
    }
}