package com.example.mymeds.data.repository
import com.example.mymeds.data.model.Take

object TakeRepository {
    private val takes = mutableListOf<Take>()

    fun getTakes(): List<Take> {
        return takes
    }

    fun addTake(take: Take) {
        takes.add(take)
    }

    fun removeTake(index: Int) {
        if (index in takes.indices) {
            takes.removeAt(index)
        }
    }

    fun updateTake(index: Int, take: Take) {
        if (index in takes.indices) {
            takes[index] = take
        }
    }

    fun clear() {
        takes.clear()
    }
}