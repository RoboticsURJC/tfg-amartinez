package com.example.mymeds.ui.pulse_oxigen

import androidx.fragment.app.viewModels
import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.example.mymeds.R
import com.example.mymeds.databinding.FragmentConfigPillsBinding
import com.example.mymeds.databinding.FragmentPulseOxigenBinding

class Pulse_oxigenFragment : Fragment() {

    private var _binding: FragmentPulseOxigenBinding? = null
    private val binding get() = _binding!!

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View {
        _binding = FragmentPulseOxigenBinding.inflate(inflater, container, false)
        return binding.root
    }

    override fun onDestroyView() {
        super.onDestroyView()
        _binding = null
    }
}