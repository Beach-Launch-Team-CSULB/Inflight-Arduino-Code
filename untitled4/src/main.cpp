#include <Arduino.h>
void setup() {
    Serial.begin(9600);
    Serial1.begin(9600);
}

void loop() {
    // put your main code here, to run repeatedly:
    if(Serial1.available() > 0){
        Serial.write(Serial1.read());
    }
}