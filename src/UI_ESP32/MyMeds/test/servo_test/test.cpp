#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

#define SERVOMIN  150
#define SERVOMAX  600

uint16_t pos0  = SERVOMIN;
uint16_t pos90 = (SERVOMIN + SERVOMAX) / 2;
uint16_t pos45 = (pos0 + pos90) / 2;

void setup1()
{
    Serial.begin(115200);

    Wire.begin(22,27);

    pwm.begin();
    pwm.setPWMFreq(50);

    delay(1000);
}

void loop1()
{
    Serial.println("0°");
    pwm.setPWM(0, 0, pos0);
    delay(2000);

    Serial.println("45");
    pwm.setPWM(0, 0, pos45);
    delay(2000);

    Serial.println("90");
    pwm.setPWM(0, 0, pos90);
    delay(2000);
}