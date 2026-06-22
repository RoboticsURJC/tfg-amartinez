#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

Adafruit_PWMServoDriver pwm(0x40);

const int POS_MAX = 220;
const int POS_MIN = 30;

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

    delay(2000);

    Serial.println("100 -> 300");
    moverServo(POS_MIN, POS_MAX, 10);

    delay(2000);
}

// ESCANER
// #include <Arduino.h>
// #include <Wire.h>

// void setup()
// {
//     Serial.begin(115200);
//     delay(1000);

//     Serial.println("Escaneando bus I2C...");

//     Wire.begin(22, 27);

//     byte count = 0;

//     for (byte address = 1; address < 127; address++)
//     {
//         Wire.beginTransmission(address);

//         byte error = Wire.endTransmission();

//         if (error == 0)
//         {
//             Serial.print("Dispositivo encontrado en 0x");

//             if (address < 16)
//                 Serial.print("0");

//             Serial.println(address, HEX);

//             count++;
//         }
//     }

//     if (count == 0)
//     {
//         Serial.println("No se encontraron dispositivos I2C");
//     }
//     else
//     {
//         Serial.println("Escaneo terminado");
//     }
// }

// void loop()
// {
// }
