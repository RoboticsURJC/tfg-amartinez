package com.example.mymeds.ui.pulse_oxigen

import android.content.Context
import android.graphics.Paint
import android.os.Environment
import android.graphics.pdf.PdfDocument
import com.example.mymeds.data.PulseMeasurement
import java.io.File
import java.io.FileOutputStream
import java.text.SimpleDateFormat
import java.util.Date
import java.util.Locale

object PulsePdfGenerator {

    fun generate(context: Context, measurements: List<PulseMeasurement>): File? {
        return try {
            val document = PdfDocument()
            val pageInfo =
                PdfDocument.PageInfo.Builder(
                    595,
                    842,
                    1
                ).create()

            var page = document.startPage(pageInfo)
            var canvas = page.canvas
            val paint = android.graphics.Paint()

            var y = 50

            paint.textSize = 26f
            paint.isFakeBoldText = true
            paint.color = android.graphics.Color.rgb(
                120,
                30,
                170
            )

            canvas.drawText(
                "MyMeds - Historial de pulsaciones",
                40f,
                y.toFloat(),
                paint
            )

            y += 30

            paint.textSize = 14f
            paint.isFakeBoldText = false
            paint.color = android.graphics.Color.BLACK

            val now = SimpleDateFormat(
                    "dd/MM/yyyy HH:mm",
                    Locale.getDefault()
                ).format(Date())

            canvas.drawText("Generado: $now", 40f, y.toFloat(), paint)

            val purplePaint = Paint().apply {
                color = android.graphics.Color.rgb(
                    120,
                    30,
                    170
                )
                strokeWidth = 2f
            }

            canvas.drawLine(
                40f,
                y + 10f,
                555f,
                y + 10f,
                purplePaint
            )

            y += 50

            if (measurements.isNotEmpty()) {
                val min = measurements.minOf { it.bpm }
                val max = measurements.maxOf { it.bpm }
                val avg = measurements.map { it.bpm }.average()

                paint.isFakeBoldText = true

                canvas.drawText(
                    "Total mediciones:",
                    40f,
                    y.toFloat(),
                    paint
                )

                paint.isFakeBoldText = false

                canvas.drawText(
                    measurements.size.toString(),
                    170f,
                    y.toFloat(),
                    paint
                )

                y += 25

                paint.isFakeBoldText = true
                canvas.drawText("BPM mínimo:", 40f, y.toFloat(), paint)

                paint.isFakeBoldText = false
                canvas.drawText(min.toString(), 140f, y.toFloat(), paint)

                y += 25

                paint.isFakeBoldText = true
                canvas.drawText("BPM máximo:", 40f, y.toFloat(), paint)

                paint.isFakeBoldText = false
                canvas.drawText(max.toString(), 145f, y.toFloat(), paint)

                y += 25

                paint.isFakeBoldText = true
                canvas.drawText("BPM medio:", 40f, y.toFloat(), paint)

                paint.isFakeBoldText = false
                canvas.drawText("%.1f".format(avg), 135f, y.toFloat(), paint)

                y += 80


                if (measurements.size >= 2) {

                    val graphLeft = 70f
                    val graphTop = y.toFloat() + 20f

                    val graphWidth = 480f
                    val graphHeight = 220f

                    val graphTitlePaint = Paint().apply {
                        color = android.graphics.Color.rgb(
                            120,
                            30,
                            170
                        )
                        textSize = 18f
                        isFakeBoldText = true
                        isAntiAlias = true
                    }

                    canvas.drawText(
                        "Evolución de pulsaciones",
                        graphLeft - 30,
                        graphTop - 45,
                        graphTitlePaint
                    )

                    val minBpm =
                        measurements.minOf { it.bpm }

                    val maxBpm =
                        measurements.maxOf { it.bpm }

                    val axisStep = 3

                    val axisMin =
                        ((minBpm / axisStep) * axisStep) - axisStep

                    val axisMax =
                        ((maxBpm + axisStep - 1) / axisStep) * axisStep

                    val bpmRange =
                        maxOf(1, axisMax - axisMin)

                    val axisPaint = Paint().apply {
                        color = android.graphics.Color.rgb(
                            120,
                            30,
                            170
                        )
                        style = Paint.Style.STROKE
                        strokeWidth = 1.5f
                        isAntiAlias = true
                    }

                    val linePaint = Paint().apply {
                        color = android.graphics.Color.rgb(
                            160,
                            50,
                            200
                        )
                        strokeWidth = 4f
                        isAntiAlias = true
                    }

                    val pointPaint = Paint().apply {
                        color = android.graphics.Color.rgb(
                            160,
                            50,
                            200
                        )
                        isAntiAlias = true
                    }

                    val textPaint = Paint().apply {
                        color = android.graphics.Color.rgb(
                            120,
                            30,
                            170
                        )
                        textSize = 15f
                        isFakeBoldText = true
                        isAntiAlias = true
                    }

                    val backgroundPaint = Paint().apply {
                        color = android.graphics.Color.WHITE
                        style = Paint.Style.FILL
                    }

                    canvas.drawText(
                        "BPM",
                        graphLeft - 30,
                        graphTop - 20,
                        graphTitlePaint
                    )

                    canvas.drawLine(
                        graphLeft,
                        graphTop + graphHeight,
                        graphLeft + graphWidth,
                        graphTop + graphHeight,
                        axisPaint
                    )

                    canvas.drawLine(
                        graphLeft,
                        graphTop,
                        graphLeft,
                        graphTop + graphHeight,
                        axisPaint
                    )

                    val gridPaint = Paint().apply {
                        color = android.graphics.Color.rgb(
                            220,
                            220,
                            220
                        )
                        strokeWidth = 1f
                        isAntiAlias = true
                    }

                    var previousX = 0f
                    var previousY = 0f

                    measurements.forEachIndexed { index, measurement ->

                        val graphInnerWidth = graphWidth - 40f

                        val x =
                            graphLeft + 20f +
                                    (index.toFloat() /
                                            (measurements.size - 1)) *
                                    graphInnerWidth

                        val yPoint =
                            graphTop +
                                    graphHeight -
                                    ((measurement.bpm - axisMin).toFloat() /
                                            bpmRange) *
                                    graphHeight

                        if (index > 0) {

                            canvas.drawLine(
                                previousX,
                                previousY,
                                x,
                                yPoint,
                                linePaint
                            )
                        }

                        canvas.drawCircle(
                            x,
                            yPoint,
                            6f,
                            pointPaint
                        )

                        canvas.drawLine(
                            x,
                            graphTop + graphHeight - 5,
                            x,
                            graphTop + graphHeight + 5,
                            axisPaint
                        )

                        canvas.drawText(
                            measurement.bpm.toString(),
                            x - 10,
                            yPoint - 10,
                            textPaint
                        )

                        previousX = x
                        previousY = yPoint
                    }

                    var value = axisMin + axisStep

                    while (value <= axisMax) {

                        val ratio =
                            (value - axisMin).toFloat() /
                                    (axisMax - axisMin)

                        val labelY =
                            graphTop +
                                    graphHeight -
                                    ratio * graphHeight

                        canvas.drawText(
                            value.toString(),
                            graphLeft - 30,
                            labelY + 5,
                            textPaint
                        )

                        canvas.drawLine(
                            graphLeft - 5,
                            labelY,
                            graphLeft + 5,
                            labelY,
                            axisPaint
                        )

                        var dashX = graphLeft

                        while (dashX < graphLeft + graphWidth) {

                            canvas.drawLine(
                                dashX,
                                labelY,
                                dashX + 8f,
                                labelY,
                                gridPaint
                            )

                            dashX += 16f
                        }

                        value += axisStep
                    }

                    y += graphHeight.toInt() + 80
                }
            }

            val TitlePaint = Paint().apply {
                color = android.graphics.Color.rgb(
                    120,
                    30,
                    170
                )
                textSize = 16f
                isFakeBoldText = true
                isAntiAlias = true
            }

            canvas.drawText("Historial", 40f, y.toFloat(), TitlePaint)

            y += 30

            canvas.drawText("Fecha", 40f, y.toFloat(), TitlePaint)
            canvas.drawText("Hora", 170f, y.toFloat(), TitlePaint)
            canvas.drawText("BPM", 300f, y.toFloat(), TitlePaint)

            y += 25

            paint.isFakeBoldText = false

            val separatorPaint = Paint().apply {
                color = android.graphics.Color.rgb(
                    220,
                    220,
                    220
                )
                strokeWidth = 1f
            }

            measurements.reversed().forEach { measurement ->

                if (y > 780)
                {
                    document.finishPage(page)

                    page =
                        document.startPage(pageInfo)

                    canvas =
                        page.canvas

                    y = 50
                    
                    canvas.drawText(
                        "Historial (continuación)",
                        40f,
                        y.toFloat(),
                        TitlePaint
                    )

                    y += 30

                    canvas.drawText(
                        "Fecha",
                        40f,
                        y.toFloat(),
                        TitlePaint
                    )

                    canvas.drawText(
                        "Hora",
                        170f,
                        y.toFloat(),
                        TitlePaint
                    )

                    canvas.drawText(
                        "BPM",
                        300f,
                        y.toFloat(),
                        TitlePaint
                    )

                    y += 25
                }

                canvas.drawText(
                    measurement.date,
                    40f,
                    y.toFloat(),
                    paint
                )

                canvas.drawText(
                    measurement.time,
                    170f,
                    y.toFloat(),
                    paint
                )

                canvas.drawText(
                    measurement.bpm.toString(),
                    300f,
                    y.toFloat(),
                    paint
                )

                canvas.drawLine(
                    40f,
                    y + 8f,
                    500f,
                    y + 8f,
                    separatorPaint
                )

                y += 25
            }

            document.finishPage(page)
            val dir = File(context.getExternalFilesDir(Environment.DIRECTORY_DOCUMENTS), "pdf")

            if (!dir.exists()) {
                dir.mkdirs()
            }

            val file = File(dir, "PulseHistory.pdf")

            document.writeTo(
                FileOutputStream(file)
            )

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

            document.close()
            file



        } catch (e: Exception) {
            e.printStackTrace()
            null
        }
    }
}