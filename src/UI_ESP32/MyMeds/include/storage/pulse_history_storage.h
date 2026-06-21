#pragma once

#include <Arduino.h>

static const int PULSE_HISTORY_MAX = 10;

struct PulseRecord
{
    String date;
    String time;
    int bpm;
};

typedef bool (*PulseHistoryVisitor)(
    const PulseRecord &record,
    void *context
);

int loadPulseHistory(
    PulseRecord records[],
    int maxRecords
);

bool pulseHistoryBegin();
bool savePulseAverage(int bpm);
bool visitPulseHistory(PulseHistoryVisitor visitor,void *context);
