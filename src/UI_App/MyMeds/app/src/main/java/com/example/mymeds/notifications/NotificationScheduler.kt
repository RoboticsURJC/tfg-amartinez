package com.example.mymeds.notifications

import android.app.AlarmManager
import android.app.PendingIntent
import android.content.Context
import android.content.Intent
import java.util.Calendar
import com.example.mymeds.data.repository.TakeRepository

object NotificationScheduler {

    fun scheduleAll(context: Context) {

        cancelAll(context)

        TakeRepository.getTakes().forEachIndexed { takeIndex, take ->

            val parts = take.time.split(":")

            val hour = parts[0].toInt()
            val minute = parts[1].toInt()

            take.days.forEachIndexed { dayIndex, day ->

                val calendar = getNextOccurrence(
                    day = dayToCalendar(day),
                    hour = hour,
                    minute = minute
                )

                val requestCode =
                    takeIndex * 100 +
                            dayIndex * 2

                scheduleNotification(
                    context = context,
                    calendar = calendar,
                    requestCode = requestCode,
                    title = "MyMeds",
                    message = "",
                    takeIndex = takeIndex,
                    dayIndex = dayIndex,
                    isReminder = false
                )

                if (
                    take.reminderEnabled &&
                    take.advanceWarningMinutes != null
                ) {

                    val reminderCalendar =
                        calendar.clone() as Calendar

                    reminderCalendar.add(
                        Calendar.MINUTE,
                        -take.advanceWarningMinutes
                    )

                    if (reminderCalendar.after(Calendar.getInstance())) {

                        scheduleNotification(
                            context = context,
                            calendar = reminderCalendar,
                            requestCode = requestCode + 1,
                            title = "MyMeds",
                            message = "",
                            takeIndex = takeIndex,
                            dayIndex = dayIndex,
                            isReminder = true
                        )
                    }
                }
            }
        }
    }

    fun cancelAll(context: Context) {

        val alarmManager =
            context.getSystemService(Context.ALARM_SERVICE) as AlarmManager

        for (requestCode in 0..1000) {

            val pendingIntent =
                PendingIntent.getBroadcast(
                    context,
                    requestCode,
                    Intent(context, AlarmReceiver::class.java),
                    PendingIntent.FLAG_NO_CREATE or PendingIntent.FLAG_IMMUTABLE
                )

            if (pendingIntent != null) {

                alarmManager.cancel(pendingIntent)
                pendingIntent.cancel()
            }
        }
    }

    private fun createPendingIntent(
        context: Context,
        requestCode: Int,
        title: String,
        message: String,
        takeIndex: Int,
        dayIndex: Int,
        isReminder: Boolean
    ): PendingIntent {

        val intent =
            Intent(context, AlarmReceiver::class.java).apply {

                putExtra("title", title)
                putExtra("message", message)

                putExtra("takeIndex", takeIndex)
                putExtra("dayIndex", dayIndex)
                putExtra("isReminder", isReminder)

                putExtra("notificationId", requestCode)
            }

        return PendingIntent.getBroadcast(
            context,
            requestCode,
            intent,
            PendingIntent.FLAG_UPDATE_CURRENT or PendingIntent.FLAG_IMMUTABLE
        )
    }

    private fun dayToCalendar(day: com.example.mymeds.data.model.DayOfWeek): Int {

        return when (day) {

            com.example.mymeds.data.model.DayOfWeek.MONDAY -> Calendar.MONDAY
            com.example.mymeds.data.model.DayOfWeek.TUESDAY -> Calendar.TUESDAY
            com.example.mymeds.data.model.DayOfWeek.WEDNESDAY -> Calendar.WEDNESDAY
            com.example.mymeds.data.model.DayOfWeek.THURSDAY -> Calendar.THURSDAY
            com.example.mymeds.data.model.DayOfWeek.FRIDAY -> Calendar.FRIDAY
            com.example.mymeds.data.model.DayOfWeek.SATURDAY -> Calendar.SATURDAY
            com.example.mymeds.data.model.DayOfWeek.SUNDAY -> Calendar.SUNDAY
        }
    }

    private fun getNextOccurrence(
        day: Int,
        hour: Int,
        minute: Int
    ): Calendar {

        val now = Calendar.getInstance()

        val calendar = Calendar.getInstance().apply {

            set(Calendar.DAY_OF_WEEK, day)
            set(Calendar.HOUR_OF_DAY, hour)
            set(Calendar.MINUTE, minute)
            set(Calendar.SECOND, 0)
            set(Calendar.MILLISECOND, 0)
        }

        if (calendar.before(now)) {
            calendar.add(Calendar.WEEK_OF_YEAR, 1)
        }

        return calendar
    }

    private fun scheduleNotification(
        context: Context,
        calendar: Calendar,
        requestCode: Int,
        title: String,
        message: String,
        takeIndex: Int,
        dayIndex: Int,
        isReminder: Boolean
    ) {

        val alarmManager =
            context.getSystemService(Context.ALARM_SERVICE) as AlarmManager

        val pendingIntent =
            createPendingIntent(
                context = context,
                requestCode = requestCode,
                title = title,
                message = message,
                takeIndex = takeIndex,
                dayIndex = dayIndex,
                isReminder = isReminder
            )

        android.util.Log.d(
            "NOTIFICATIONS",
            "Programando alarma para ${calendar.time}"
        )

        alarmManager.setExactAndAllowWhileIdle(
            AlarmManager.RTC_WAKEUP,
            calendar.timeInMillis,
            pendingIntent
        )
    }
}