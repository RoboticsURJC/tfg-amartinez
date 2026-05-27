#pragma once

void pulseSensorInit();
void pulseSensorUpdate();
bool pulseFingerDetected();
bool pulseMeasurementReady();
int pulseGetBpm();
int pulseGetSamples();
int pulseGetRequiredSamples();
void pulseSensorStart();
void pulseSensorStop();