package com.example.mymeds

import android.app.Application

class MyMedsApplication : Application() {

    companion object {

        lateinit var instance: MyMedsApplication
            private set
    }

    override fun onCreate() {
        super.onCreate()

        instance = this
    }
}