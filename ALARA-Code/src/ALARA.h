//
// Created by shawn on 4/21/22.
//

#ifndef ALARA_CODE_ALARA_H
#define ALARA_CODE_ALARA_H
// Pin definitions for ALARAv1
// Put any other ALARA-specific pins, devices, etc. here
const uint8_t
BNO055_RST = 5, BNO055_CS = 47,
MS5607_CSN = 10,
KX_TRIG = 6,  KX_INT1 = 7, KX_INT2 = 8, KX_CSN = 9,
LORA_INT = 15, LORA_RST = 22, LORA_CSN = 23,
NRF_IRQ = 0, NRF_CE = 1, NRF_CSN = 2,
NOR1_CS = 37,NOR2_CS = 36,
DSUBIO1 = 34, DSUBIO2 = 33,
GPS_TP = 56, GPS_INT = 57,
BUZZ = 54,
CAN1_INT0 = 17, CAN1_INT1 = 16, CAN1_CLK = 40, CAN1_CSN = 41,  CAN1_INT = 42,
CAN2_CLK = 3, CAN2_INT = 4, CAN2_CS = 26, CAN2_INT0 = 27, CAN2_INT1 = 28;
#endif //ALARA_CODE_ALARA_H