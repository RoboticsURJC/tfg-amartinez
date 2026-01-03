package com.example.mymeds.ui.welcome

import android.os.Bundle
import android.os.Handler
import android.os.Looper
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.example.mymeds.R
import androidx.navigation.fragment.findNavController

class WelcomeFragment : Fragment() {
    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_welcome, container, false)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        Handler(Looper.getMainLooper()).postDelayed({
            findNavController().navigate(R.id.homeFragment,
                null,
                androidx.navigation.NavOptions.Builder()
                    .setPopUpTo(R.id.welcomeFragment, true)
                    .build()
            )}, 4000)
    }

    override fun onResume() {
        super.onResume()
        (requireActivity() as androidx.appcompat.app.AppCompatActivity).supportActionBar?.hide()
    }
}