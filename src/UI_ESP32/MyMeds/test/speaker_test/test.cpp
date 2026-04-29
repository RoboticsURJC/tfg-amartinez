#include <Arduino.h>

#define SPEAKER_PIN 26
#define CHANNEL 0

#define VOL_MAX 70
#define VOL_MIN 10

// ------------------------
// INIT
// ------------------------
void setup() {
    Serial.begin(115200);

    ledcSetup(CHANNEL, 1000, 8);
    ledcAttachPin(SPEAKER_PIN, CHANNEL);

    Serial.println("=== TEST ARMONIA ===");
}

// ------------------------
// FADE
// ------------------------
void fadeIn() {
    for (int d = VOL_MIN; d <= VOL_MAX; d += 5) {
        ledcWrite(CHANNEL, d);
        delay(4);
    }
}

void fadeOut() {
    for (int d = VOL_MAX; d >= 0; d -= 5) {
        ledcWrite(CHANNEL, d);
        delay(4);
    }
}

// ------------------------
// NOTA
// ------------------------
void playNote(int freq, int duration) {
    ledcWriteTone(CHANNEL, freq);
    fadeIn();
    delay(duration);
    fadeOut();
    ledcWriteTone(CHANNEL, 0);
}

// ------------------------
// SONIDOS
// ------------------------

// 🔔 Aviso previo (muy suave)
void warning_sound() {
    Serial.println("WARNING");

    playNote(523, 150); // DO
    delay(80);
    playNote(659, 150); // MI
    delay(60);

    playNote(784, 220);
}

// 💊 Alarma agradable (armonía)
void alarm_sound() {
    Serial.println("ALARM");

    // DO → MI → SOL
    playNote(523, 180);
    delay(60);

    playNote(659, 180);
    delay(60);

    playNote(784, 220);
    delay(60);

    playNote(659, 180);
    delay(30);

    playNote(784, 220);
}

// 🔁 ciclo completo
void alarm_cycle() {
    for (int i = 0; i < 4; i++) {
        alarm_sound();
        delay(2000);
    }
}

// ------------------------
// LOOP
// ------------------------
void loop() {

    Serial.println("\n--- CICLO ---");

    warning_sound();   // aviso previo
    delay(3000);

    alarm_cycle();     // alarma real

    delay(8000);
}