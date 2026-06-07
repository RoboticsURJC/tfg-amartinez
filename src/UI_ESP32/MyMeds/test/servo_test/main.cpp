// #include <Arduino.h>
// #include <Wire.h>
// #include <Adafruit_PWMServoDriver.h>

// Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// #define SERVOMIN  150
// #define SERVOMAX  600

// uint16_t pos0  = SERVOMIN;
// uint16_t pos90 = (SERVOMIN + SERVOMAX) / 2;
// uint16_t pos45 = (pos0 + pos90) / 2;

// void setup1()
// {
//     Serial.begin(115200);

//     Wire.begin(22,27);

//     pwm.begin();
//     pwm.setPWMFreq(50);

//     delay(1000);
// }

// void loop1()
// {
//     Serial.println("0°");
//     pwm.setPWM(0, 0, pos0);
//     delay(2000);

//     Serial.println("45");
//     pwm.setPWM(0, 0, pos45);
//     delay(2000);

//     Serial.println("90");
//     pwm.setPWM(0, 0, pos90);
//     delay(2000);
// }

// #include <Arduino.h>
// #include <Wire.h>
// #include <Adafruit_PWMServoDriver.h>

// Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// #define SERVOMIN 150
// #define SERVOMAX 600

// uint16_t pos0  = SERVOMIN;
// uint16_t pos90 = (SERVOMIN + SERVOMAX) / 2;
// uint16_t pos45 = (pos0 + pos90) / 2;
// uint16_t pos60 =
//     pos0 -
//     ((pos90 - pos0) * 60) / 90;
// uint16_t center = 375;
// uint16_t posMinus50 =
//     center - ((pos90 - pos0) * 50) / 90;

// void moveServoSlow(uint16_t from, uint16_t to)
// {
//     if (from < to)
//     {
//         for (uint16_t p = from; p <= to; p++)
//         {
//             pwm.setPWM(0, 0, p);
//             delay(15);   // más alto = más lento
//         }
//     }
//     else
//     {
//         for (int p = from; p >= (int)to; p--)
//         {
//             pwm.setPWM(0, 0, p);
//             delay(15);
//         }
//     }
// }

// void setup()
// {
//     Serial.begin(115200);

//     Wire.begin(22, 27);

//     pwm.begin();
//     pwm.setPWMFreq(50);

//     delay(1000);

//     // Posición inicial
//     pwm.setPWM(0, 0, pos0);
//     delay(1000);
// }

// void loop()
// {
//     Serial.println("Moviendo a 45°");

//     moveServoSlow(pos0, 150);

//     delay(1000);

//     Serial.println("Volviendo a 0°");

//     moveServoSlow(150, pos0);

//     delay(2000);
// }

// #include <Arduino.h>
// #include <Wire.h>
// #include <Adafruit_PWMServoDriver.h>

// Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(0x40);

// #define SERVOMIN 150

// void setup()
// {
//     Serial.begin(115200);

//     Wire.begin(22, 27);

//     pwm.begin();
//     pwm.setPWMFreq(50);

//     delay(1000);

//     Serial.println("Moviendo a 0°");

//     pwm.setPWM(0, 0, SERVOMIN);
// }

// void loop()
// {
// }

// #include <Arduino.h>
// #include <Wire.h>
// #include <Adafruit_PWMServoDriver.h>

// Adafruit_PWMServoDriver pwm(0x40);

// void setup()
// {
//     Serial.begin(115200);

//     Wire.begin(22,27);

//     pwm.begin();
//     pwm.setPWMFreq(50);

//     delay(1000);

//     //pwm.setPWM(0, 0, 300);
// }
// void loop()
// {

//     // for(int p = 300; p >= 90; p -= 20)
//     // {
//     //     Serial.println(p);
//     //     pwm.setPWM(0, 0, p);
//     //     delay(2000);
//     // }
//     Serial.println("300");
//     pwm.setPWM(0,0,300);
//     delay(6000);

//     Serial.println("100");
//     pwm.setPWM(0,0,100);
//     delay(6000);

//     // Serial.println("300");
//     // pwm.setPWM(0,0,300);
//     // delay(3000);

//     // Serial.println("350");
//     // pwm.setPWM(0,0,350);
//     // delay(3000);

//     // Serial.println("400");
//     // pwm.setPWM(0,0,400);
//     // delay(3000);
// }

#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm(0x40);

const int POS_MAX = 220;
const int POS_MIN = 100;

void moverServo(int inicio, int fin, int velocidad_ms)
{
    if(inicio < fin)
    {
        for(int p = inicio; p <= fin; p++)
        {
            pwm.setPWM(0, 0, p);
            delay(velocidad_ms);
        }
    }
    else
    {
        for(int p = inicio; p >= fin; p--)
        {
            pwm.setPWM(0, 0, p);
            delay(velocidad_ms);
        }
    }
}

void setup()
{
    Serial.begin(115200);

    Wire.begin(22, 27);

    pwm.begin();
    pwm.setPWMFreq(50);

    delay(1000);

    //pwm.setPWM(0, 0, 300);
}

void loop()
{
    Serial.println("300 -> 100");
    moverServo(POS_MAX, POS_MIN, 5);

    delay(500);

    Serial.println("100 -> 300");
    moverServo(POS_MIN, POS_MAX, 10);

    delay(500);
}
