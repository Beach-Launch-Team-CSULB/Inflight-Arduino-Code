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
    analogWriteFrequency(20,440);
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
analowWrite(20, 10);
}

