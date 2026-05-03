package com.example.mymeds.data.repository
 import com.example.mymeds.data.model.Medicine

object MedicineRepository {
    private val medicines = mutableListOf<Medicine>()

    fun getAll(): List<Medicine> = medicines

    fun setAll(list: List<Medicine>) {
        medicines.clear()
        medicines.addAll(list)
    }

    fun add(medicine: Medicine) {
        medicines.add(medicine)
    }

    fun findById(id: String): Medicine? {
        return medicines.find { it.id == id }
    }

    fun removeAt(position: Int) {
        medicines.removeAt(position)
    }
}