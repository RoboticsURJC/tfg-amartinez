#include "actuators/dose_actuators.h"

#include <Adafruit_PWMServoDriver.h>
#include <Arduino.h>

static const int SPEAKER_PIN = 26;
static const int SPEAKER_CHANNEL = 0;
static const int SPEAKER_VOLUME = 70;

static const int SERVO_CHANNEL = 0;
static const int SERVO_POS_MAX = 220;
static const int SERVO_POS_MIN = 30;
static const int SERVO_ACTIONS = 4;

static const int NOTE_FREQUENCIES[] = {523, 659, 784, 659, 784};
static const int NOTE_DURATIONS[] = {180, 180, 220, 180, 220};
static const int NOTE_GAPS[] = {60, 60, 60, 30, 0};
static const int NOTE_COUNT = 5;

enum ServoPhase
{
    SERVO_IDLE,
    SERVO_MOVING_TO_MIN,
    SERVO_HOLD_MIN,
    SERVO_MOVING_TO_MAX,
    SERVO_HOLD_MAX,
    SERVO_DONE
};

enum SpeakerPhase
{
    SPEAKER_IDLE,
    SPEAKER_NOTE,
    SPEAKER_NOTE_GAP,
    SPEAKER_CYCLE_GAP,
    SPEAKER_DONE
};

static Adafruit_PWMServoDriver pwm(0x40);
static bool initialized = false;
static bool running = false;

static ServoPhase servoPhase = SERVO_IDLE;
static int servoPosition = SERVO_POS_MAX;
static int servoActionCount = 0;
static unsigned long servoNextUpdate = 0;

static SpeakerPhase speakerPhase = SPEAKER_IDLE;
static int speakerNote = 0;
static int speakerCycle = 0;
static unsigned long speakerNextUpdate = 0;

static void startSpeakerNote(unsigned long now)
{
    ledcWriteTone(SPEAKER_CHANNEL, NOTE_FREQUENCIES[speakerNote]);
    ledcWrite(SPEAKER_CHANNEL, SPEAKER_VOLUME);
    speakerPhase = SPEAKER_NOTE;
    speakerNextUpdate = now + NOTE_DURATIONS[speakerNote];
}

void doseActuatorsInit()
{
    if (initialized)
    {
        return;
    }

    pwm.begin();
    pwm.setPWMFreq(50);
    pwm.setPWM(SERVO_CHANNEL, 0, SERVO_POS_MAX);

    ledcSetup(SPEAKER_CHANNEL, 1000, 8);
    ledcAttachPin(SPEAKER_PIN, SPEAKER_CHANNEL);
    ledcWrite(SPEAKER_CHANNEL, 0);

    initialized = true;
}

void doseActuatorsStart()
{
    doseActuatorsInit();

    if (running)
    {
        return;
    }

    unsigned long now = millis();

    running = true;
    servoPosition = SERVO_POS_MAX;
    servoActionCount = 0;
    servoPhase = SERVO_MOVING_TO_MIN;
    servoNextUpdate = now;

    speakerNote = 0;
    speakerCycle = 0;
    startSpeakerNote(now);
}

static void updateServo(unsigned long now)
{
    if (now < servoNextUpdate)
    {
        return;
    }

    switch (servoPhase)
    {
        case SERVO_MOVING_TO_MIN:
            if (servoPosition > SERVO_POS_MIN)
            {
                servoPosition--;
                pwm.setPWM(SERVO_CHANNEL, 0, servoPosition);
                servoNextUpdate = now + 5;
            }
            else
            {
                servoPhase = SERVO_HOLD_MIN;
                servoNextUpdate = now + 2000;
            }
            break;

        case SERVO_HOLD_MIN:
            servoPhase = SERVO_MOVING_TO_MAX;
            servoNextUpdate = now;
            break;

        case SERVO_MOVING_TO_MAX:
            if (servoPosition < SERVO_POS_MAX)
            {
                servoPosition++;
                pwm.setPWM(SERVO_CHANNEL, 0, servoPosition);
                servoNextUpdate = now + 10;
            }
            else
            {
                servoActionCount++;
                servoPhase = SERVO_HOLD_MAX;
                servoNextUpdate = now + 2000;
            }
            break;

        case SERVO_HOLD_MAX:
            if (servoActionCount >= SERVO_ACTIONS)
            {
                servoPhase = SERVO_DONE;
            }
            else
            {
                servoPhase = SERVO_MOVING_TO_MIN;
                servoNextUpdate = now;
            }
            break;

        default:
            break;
    }
}

static void updateSpeaker(unsigned long now)
{
    if (now < speakerNextUpdate)
    {
        return;
    }

    switch (speakerPhase)
    {
        case SPEAKER_NOTE:
            ledcWrite(SPEAKER_CHANNEL, 0);
            ledcWriteTone(SPEAKER_CHANNEL, 0);
            speakerPhase = SPEAKER_NOTE_GAP;
            speakerNextUpdate = now + NOTE_GAPS[speakerNote];
            break;

        case SPEAKER_NOTE_GAP:
            speakerNote++;

            if (speakerNote < NOTE_COUNT)
            {
                startSpeakerNote(now);
            }
            else
            {
                speakerCycle++;

                if (speakerCycle >= 4)
                {
                    speakerPhase = SPEAKER_DONE;
                }
                else
                {
                    speakerPhase = SPEAKER_CYCLE_GAP;
                    speakerNextUpdate = now + 2000;
                }
            }
            break;

        case SPEAKER_CYCLE_GAP:
            speakerNote = 0;
            startSpeakerNote(now);
            break;

        default:
            break;
    }
}

void doseActuatorsUpdate()
{
    if (!running)
    {
        return;
    }

    unsigned long now = millis();
    updateServo(now);
    updateSpeaker(now);

    if (servoPhase == SERVO_DONE && speakerPhase == SPEAKER_DONE)
    {
        running = false;
    }
}

void doseActuatorsStop()
{
    ledcWrite(SPEAKER_CHANNEL, 0);
    ledcWriteTone(SPEAKER_CHANNEL, 0);

    if (initialized)
    {
        servoPosition = SERVO_POS_MAX;
        pwm.setPWM(SERVO_CHANNEL, 0, SERVO_POS_MAX);
    }

    servoPhase = SERVO_IDLE;
    speakerPhase = SPEAKER_IDLE;
    running = false;
}

void doseActuatorsAcknowledge()
{
    ledcWrite(SPEAKER_CHANNEL, 0);
    ledcWriteTone(SPEAKER_CHANNEL, 0);
    speakerPhase = SPEAKER_DONE;

    if (servoPhase == SERVO_DONE)
    {
        running = false;
    }
}

bool doseActuatorsBusy()
{
    return running;
}
