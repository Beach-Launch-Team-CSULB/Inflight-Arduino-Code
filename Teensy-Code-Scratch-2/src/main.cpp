#include <Arduino.h>
#include <InternalTemperature.h>
void setup()
{

}
void loop()
{
Serial.println(InternalTemperatureClass::readTemperatureC(), 50);
delay(1000);
}

