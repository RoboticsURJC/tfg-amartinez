package com.example.mymeds.ui.pulse_oxigen

import android.content.Context
import android.graphics.Bitmap
import android.os.Environment
import android.graphics.pdf.PdfDocument
import com.example.mymeds.data.PulseMeasurement
import java.io.File
import java.io.FileOutputStream
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

object PulsePdfGenerator {

    fun generate(context: Context, measurements: List<PulseMeasurement>, chartBitmap: Bitmap?
    ): File? {
        return try {
            val document = PdfDocument()
            val pageInfo =
                PdfDocument.PageInfo.Builder(
                    595,
                    842,
                    1
                ).create()

            val page = document.startPage(pageInfo)
            val canvas = page.canvas
            val paint = android.graphics.Paint()

            var y = 50

            paint.textSize = 24f
            paint.isFakeBoldText = true

            canvas.drawText(
                "MyMeds - Historial de pulsaciones",
                40f,
                y.toFloat(),
                paint
            )

            y += 40

            paint.textSize = 14f
            paint.isFakeBoldText = false

            val now = SimpleDateFormat(
                    "dd/MM/yyyy HH:mm",
                    Locale.getDefault()
                ).format(Date())

            canvas.drawText("Generado: $now", 40f, y.toFloat(), paint)

            y += 40

            if (measurements.isNotEmpty()) {
                val min = measurements.minOf { it.bpm }
                val max = measurements.maxOf { it.bpm }
                val avg = measurements.map { it.bpm }.average()

                canvas.drawText("Total mediciones: ${measurements.size}", 40f, y.toFloat(), paint)

                y += 25

                canvas.drawText("BPM mínimo: $min", 40f, y.toFloat(), paint)

                y += 25

                canvas.drawText("BPM máximo: $max", 40f, y.toFloat(), paint)

                y += 25

                canvas.drawText("BPM medio: %.1f".format(avg), 40f, y.toFloat(), paint)

                y += 50

                if (chartBitmap != null) {

                    paint.isAntiAlias = true
                    paint.isFilterBitmap = true
                    paint.isDither = true

                    val maxWidth = 500

                    val ratio =
                        chartBitmap.height.toFloat() /
                                chartBitmap.width.toFloat()

                    val height =
                        (maxWidth * ratio).toInt()

                    val scaledBitmap =
                        Bitmap.createScaledBitmap(
                            chartBitmap,
                            maxWidth,
                            height,
                            true
                        )

                    canvas.drawBitmap(
                        scaledBitmap,
                        40f,
                        y.toFloat(),
                        paint
                    )

                    y += height + 30
                }
            }

            paint.isFakeBoldText = true

            canvas.drawText("Historial", 40f, y.toFloat(), paint)

            y += 30

            paint.isFakeBoldText = false
            paint.textSize = 12f

            measurements.reversed().forEach {

                canvas.drawText("${it.date} ${it.time}    ${it.bpm} BPM", 40f, y.toFloat(), paint)

                y += 20

                if (y > 780) {
                    return@forEach
                }

            }

            document.finishPage(page)
            val dir = File(context.getExternalFilesDir(Environment.DIRECTORY_DOCUMENTS), "pdf")

            if (!dir.exists()) {
                dir.mkdirs()
            }

            val file = File(dir, "PulseHistory.pdf")
            android.util.Log.d(
                "PDF_TEST",
                "Ruta: ${file.absolutePath}"
            )

            android.util.Log.d(
                "PDF_TEST",
                "Existe: ${file.exists()}"
            )

            android.util.Log.d(
                "PDF_TEST",
                "Tamaño: ${file.length()}"
            )
            document.writeTo(FileOutputStream(file))
            document.close()
            file



        } catch (e: Exception) {
            e.printStackTrace()
            null
        }
    }
}