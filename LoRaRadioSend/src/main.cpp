// Teensy 3.6 and LoRa 868 (Transmitter)
// Copyright (C) 2022 https://www.roboticboat.uk
// e8e136c8-0250-4be3-8ee1-3a88a4400a6c
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.
// These Terms shall be governed and construed in accordance with the laws of
// England and Wales, without regard to its conflict of law provisions.


#include <SPI.h>
#include <RH_RF95.h>

#define RFM95_CS  10
#define RFM95_RST 15
#define RFM95_INT 14

// LoRa radio frequency
#define RF95_FREQ 868.0

RH_RF95 rf95(RFM95_CS, RFM95_INT);

int i=0;
int16_t packetnum = 0;

unsigned long mytime;
unsigned long lasttime;

char message[250];
char radiopacket[250];

void setup()
{
    // Keep the User informed
    Serial.begin(9600);

    delay(1000);

    // Initialise the RFM95 pins
    pinMode(RFM95_RST, OUTPUT);

    // Red LED pin
    pinMode(31, OUTPUT);

    // Green LED pin
    pinMode(32, OUTPUT);

    // Set Reset pin to high
    digitalWrite(RFM95_RST, HIGH);
    delay(100);

    // Set Reset pin to low
    digitalWrite(RFM95_RST, LOW);
    delay(100);

    // Set Reset pin to high (again)
    digitalWrite(RFM95_RST, HIGH);
    delay(100);

    // Did the RFM95 start ok?
    if (!rf95.init())
    {
        // Failed to start correctly
        Serial.println("LoRa radio init failed");

        // Turn-on red led for 1 second
        digitalWrite(31, HIGH);

        while (1);
    }

    // All ok
    Serial.println("LoRa radio init OK!");

    // Set the frequency we are using
    if (!rf95.setFrequency(RF95_FREQ))
    {
        // Failed to set frequency
        Serial.println("setFrequency failed");
        while (1);
    }

    // All ok
    Serial.print("Set Freq to: "); Serial.println(RF95_FREQ);

    // Turn-on green led for 1 second
    digitalWrite(32, HIGH);
    delay(1000);
    digitalWrite(32, LOW);

    // Set transmitter power to max dBm:
    rf95.setTxPower(20, false);

    //Set the last time
    lasttime = 0;
}

int MakeChecksum(char* msg){

    // Length of the GPS message
    int len = strlen(msg);

    // Initialise the checksum
    int cksum = 0;

    // Loop over message characters
    for (int i=0; i < len; i++) {
        cksum ^= msg[i];
    }

    return cksum % 0xffff;
}

void loop()
{
    // Timer
    mytime = millis();

    // Construct a message to send
    sprintf(message,"$MSG,%lu,113,0,-10,51.44117737,0.26763833,12,1,2,3,4",mytime);

    // Add a checksum
    sprintf(radiopacket,"%s*%02X",message,MakeChecksum(message));

    // Has 500 milliseconds passed yet?
    if (abs(mytime - lasttime) > 500)
    {
        // So we enter this section every 1 second
        lasttime = mytime;

        // Transmit message (strlen rather than sizeof)
        rf95.send((uint8_t *)radiopacket, (int)strlen(radiopacket));

        // Waits until the current message has been transmitted
        rf95.waitPacketSent();

        // Now wait for a reply
        uint8_t buf[RH_RF95_MAX_MESSAGE_LEN];
        uint8_t len = sizeof(buf);

        // Timeout in milliseconds
        if (rf95.waitAvailableTimeout(500))
        {

            // Should be a reply message for us now
            if (rf95.recv(buf, &len))
            {
                if (Serial){
                    Serial.print("[");
                    Serial.print(strlen(radiopacket));
                    Serial.print("<=");
                    Serial.print(RH_RF95_MAX_MESSAGE_LEN);
                    Serial.print("][");
                    Serial.print(radiopacket);
                    Serial.print("] ");
                    Serial.print((char*)buf);
                    Serial.print(" RSSI: ");
                    Serial.print(rf95.lastRssi(), DEC);
                    Serial.println(",");
                }

                // Turn-on green led for 100 ms
                digitalWrite(32, HIGH);
                delay(100);
                digitalWrite(32, LOW);
            }
        }
        else
        {
            // Transmission error
            if (Serial){
                Serial.println("No confirm of receipt");
            }

            // Turn-on red led for 100 ms
            digitalWrite(31, HIGH);
            delay(100);
            digitalWrite(31, LOW);
        }

    }

    delay(10);

}

