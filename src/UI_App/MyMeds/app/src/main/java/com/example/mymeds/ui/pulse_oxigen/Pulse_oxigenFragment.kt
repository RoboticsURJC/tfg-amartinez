package com.example.mymeds.ui.pulse_oxigen

import androidx.fragment.app.viewModels
import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.util.Log
import com.example.mymeds.databinding.FragmentPulseOxigenBinding
import com.example.mymeds.data.network.EspApi
import com.example.mymeds.data.PulseMeasurement
import com.example.mymeds.repository.PulseHistoryRepository
import org.json.JSONObject
import androidx.recyclerview.widget.LinearLayoutManager
import com.github.mikephil.charting.data.Entry
import com.github.mikephil.charting.data.LineData
import com.github.mikephil.charting.data.LineDataSet
import java.util.Date
import java.util.Locale
import java.text.SimpleDateFormat
import android.app.DatePickerDialog
import androidx.navigation.fragment.findNavController
import java.util.Calendar
import android.content.Intent
import androidx.core.content.FileProvider
import android.graphics.Bitmap
import android.graphics.Canvas

class Pulse_oxigenFragment : Fragment() {

    private var _binding: FragmentPulseOxigenBinding? = null
    private val binding get() = _binding!!

    private var allMeasurements = listOf<PulseMeasurement>()
    private var startDate: Date? = null
    private var endDate: Date? = null

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentPulseOxigenBinding.inflate(inflater, container, false)

        binding.buttonBack.setOnClickListener {
            findNavController().navigateUp()
        }

        binding.btnStartDate.setOnClickListener {
            val calendar = Calendar.getInstance()
            DatePickerDialog(requireContext(),
                { _, year, month, day ->
                    calendar.set(year, month, day, 0, 0, 0)
                    startDate = calendar.time
                    binding.btnStartDate.text = String.format(
                            "%02d/%02d/%04d",
                            day,
                            month + 1,
                            year
                        )
                },
                calendar.get(Calendar.YEAR),
                calendar.get(Calendar.MONTH),
                calendar.get(Calendar.DAY_OF_MONTH)
            ).show()
        }

        binding.btnEndDate.setOnClickListener {
            val calendar = Calendar.getInstance()
            DatePickerDialog(requireContext(),
                { _, year, month, day ->

                    calendar.set(year, month, day, 23, 59, 59)
                    endDate = calendar.time

                    binding.btnEndDate.text = String.format(
                            "%02d/%02d/%04d",
                            day,
                            month + 1,
                            year
                        )
                },
                calendar.get(Calendar.YEAR),
                calendar.get(Calendar.MONTH),
                calendar.get(Calendar.DAY_OF_MONTH)
            ).show()
        }

        binding.btnApplyFilter.setOnClickListener {
            applyFilter()
        }

        binding.btnClearFilter.setOnClickListener {

            startDate = null
            endDate = null

            binding.btnStartDate.text = "Desde"
            binding.btnEndDate.text = "Hasta"

            binding.historyRecycler.adapter = PulseHistoryAdapter(allMeasurements.reversed())

            updateChart(allMeasurements)
        }

        binding.btnGeneratePdf.setOnClickListener {

            val scaleFactor = 3f

            val pdfBitmap =
                Bitmap.createBitmap(
                    (binding.pulseChart.width * scaleFactor).toInt(),
                    (binding.pulseChart.height * scaleFactor).toInt(),
                    Bitmap.Config.ARGB_8888
                )

            val pdfCanvas = Canvas(pdfBitmap)

            pdfCanvas.scale(
                scaleFactor,
                scaleFactor
            )

            binding.pulseChart.draw(pdfCanvas)

            val file =
                PulsePdfGenerator.generate(
                    requireContext(),
                    allMeasurements,
                    pdfBitmap
                )

            if (file != null) {

                val uri =
                    FileProvider.getUriForFile(
                        requireContext(),
                        "${requireContext().packageName}.provider",
                        file
                    )

                val shareIntent =
                    Intent(Intent.ACTION_SEND)

                shareIntent.type = "application/pdf"

                shareIntent.putExtra(
                    Intent.EXTRA_STREAM,
                    uri
                )

                shareIntent.addFlags(
                    Intent.FLAG_GRANT_READ_URI_PERMISSION
                )

                startActivity(
                    Intent.createChooser(
                        shareIntent,
                        "Compartir informe PDF"
                    )
                )
            }
        }

        EspApi.getPulseHistory { response ->
            if (response == null) {
                return@getPulseHistory
            }

            Log.d("PULSE_HISTORY", response)

            try {

                val json = JSONObject(response)
                val measurementsArray = json.getJSONArray("measurements")
                val measurements = mutableListOf<PulseMeasurement>()

                for (i in 0 until measurementsArray.length())
                {
                    val item = measurementsArray.getJSONObject(i)
                    measurements.add(
                        PulseMeasurement(
                            date = item.getString("date"),
                            time = item.getString("time"),
                            bpm = item.getInt("bpm")
                        )
                    )
                }

                if (measurements.isEmpty()) {
                    Log.d("PULSE_HISTORY", "No hay mediciones")
                } else {
                    Log.d("PULSE_HISTORY", "Recibidas ${measurements.size} mediciones")
                    Log.d("PULSE_HISTORY", measurements.toString())
                }

                PulseHistoryRepository.setMeasurements(measurements)

                allMeasurements = measurements

                activity?.runOnUiThread {

                    binding.historyRecycler.layoutManager = LinearLayoutManager(requireContext())
                    binding.historyRecycler.isNestedScrollingEnabled = false
                    binding.historyRecycler.adapter = PulseHistoryAdapter(measurements.reversed())
                    updateChart(measurements)
                }

            } catch (e: Exception) {
                e.printStackTrace()
            }
        }

        return binding.root
    }

    private fun updateChart(measurements: List<PulseMeasurement>) {
        if (measurements.isEmpty()) {
            binding.pulseChart.clear()
            return
        }

        val entries = mutableListOf<Entry>()

        measurements.forEachIndexed { index, measurement ->
            entries.add(
                Entry(
                    index.toFloat(),
                    measurement.bpm.toFloat()
                )
            )
        }

        val dataSet = LineDataSet(entries, "BPM")
        dataSet.setDrawValues(true)
        dataSet.setDrawCircles(true)
        dataSet.lineWidth = 2f

        val lineData = LineData(dataSet)

        binding.pulseChart.data = lineData

        binding.pulseChart.description.isEnabled = false
        binding.pulseChart.legend.isEnabled = false
        binding.pulseChart.axisRight.isEnabled = false

        binding.pulseChart.axisLeft.granularity = 1f
        binding.pulseChart.axisLeft.setDrawGridLines(true)

        binding.pulseChart.xAxis.setDrawLabels(false)

        val minBpm = measurements.minOf { it.bpm }
        val maxBpm = measurements.maxOf { it.bpm }

        binding.pulseChart.axisLeft.axisMinimum = (minBpm - 5).toFloat()
        binding.pulseChart.axisLeft.axisMaximum = (maxBpm + 5).toFloat()

        binding.pulseChart.animateX(1000)
        binding.pulseChart.invalidate()
    }

    private fun applyFilter()
    {
        val sdf = SimpleDateFormat("yyyy-MM-dd", Locale.getDefault())
        val filtered = allMeasurements.filter {
                try {
                    val measurementDate = sdf.parse(it.date)
                    val afterStart = startDate == null || !measurementDate.before(startDate)
                    val beforeEnd = endDate == null || !measurementDate.after(endDate)
                    afterStart && beforeEnd

                } catch (e: Exception) {
                    false
                }
            }

        binding.historyRecycler.adapter = PulseHistoryAdapter(filtered.reversed())
        updateChart(filtered)
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}