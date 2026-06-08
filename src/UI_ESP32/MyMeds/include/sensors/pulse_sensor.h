#pragma once

void pulseSensorInit();
void pulseSensorUpdate();
bool pulseFingerDetected();
bool pulseMeasurementReady();
int pulseGetBpm();
bool pulseFullMeasurementReady();
int pulseGetFullBufferBpm();
int pulseGetSamples();
int pulseGetRequiredSamples();
void pulseSensorStart();
void pulseSensorStop();
