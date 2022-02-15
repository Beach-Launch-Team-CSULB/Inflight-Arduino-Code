#include <Arduino.h>

void intExample() {
    digitalWrite(13, HIGH);
    delay(10);
    digitalWrite(13, LOW);
}

#define test_pin 34
#define out_pin 35
void setup() {
    pinMode(35, OUTPUT);
    pinMode(34, INPUT);
    pinMode(13, OUTPUT);
    attachInterrupt(34, intExample, RISING);
}


void loop() {
    digitalWrite(out_pin, HIGH);
    Serial.print("Value high. Digital Read: ");
    Serial.println(digitalRead(test_pin));
    delay(10);
    digitalWrite(out_pin, LOW);
    Serial.print("Value low. Digital Read: ");
    Serial.println(digitalRead(test_pin));
    delay(10);
}
