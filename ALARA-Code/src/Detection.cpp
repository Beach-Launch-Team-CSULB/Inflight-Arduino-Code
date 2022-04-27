//
// Created by shawn on 3/25/22.
//

#include "Detection.h"
#include "EventLog.h"
#include <Streaming.h>

Detection::Detection() {
    launch_state = 0;
    test = false;
}

void Detection::setState(uint8_t state) {
    launch_state = state;
    test = true;
}

bool Detection::updateDetection(std::deque<bno_struct> * icm_data, std::deque<ms5607_struct> * bmp_data)
{
    bool output = false;
    //<--- Get the averages for vertical acceleration
    double sum1 = 0;
    // Number of buffer values to use for calculation
    const uint8_t icm_data_size = 64;
    const uint8_t bmp_data_size = 16;
    for(auto i = icm_data->rbegin(); distance(icm_data->rbegin(), i) < icm_data_size && i != icm_data->rend(); ++i) {
        sum1 += i->acceleration[1];
    }
    Serial.println("Finished iteration");
    double verticalAcceleration = sum1 / icm_data_size;
    //---->
    //<--- Get the averages for altitude
    sum1 = 0;
    for(auto i = bmp_data->rbegin(); distance(bmp_data->rbegin(), i) < bmp_data_size && i != bmp_data->rend(); ++i) {
        sum1 += i->alt;
        Serial.println(i->toString());
        Serial.flush();
    }
    double altitudeAvg1 = sum1 / bmp_data_size;
    //altitudeAvg2 = sum2 / (altBuff.size() / 2.0);
    //---->
    // cout<< verticalAcceleration;
    // LAUNCH CONDITIONS
    if (((launch_state == 0 && verticalAcceleration > LAUNCH_MIN_M_PER_S) &&
         icm_data->size() >= packet_size / 2 && altitudeAvg1 > ALTITUDE_LANDING_THRESHOLD) || (launch_state == 1 && test))
    {
        Serial.println("**Launch detected***");
        Serial.println(verticalAcceleration);
        launch_state = 1;
        output = true;
        test = false;
        // launchIndex = rows;
    }
    // landing altitude threshold occurs at index 1190581636 | the actual value is 4.67 IN ALT_002.csv from 08/14/2021
    // the real code should check the altitude is under 5
    // //todo Remove the line and add a condition to check the (altitude < 5)
    // LAND CONDITIONS
    if (((launch_state == 1 || launch_state == 2) && (altitudeAvg1 < ALTITUDE_LANDING_THRESHOLD &&
                                                      (verticalAcceleration > 25 || verticalAcceleration < 25 ))) || (launch_state == 3 && test))
    {
        // note: if apogee detection ends up working well could
        //  add that as a condition for land
        output = true;
        Serial.println("Test");
        Serial.println(test);
        Serial.println("AltAvg1");
        Serial.println(altitudeAvg1);
        Serial.println(ALTITUDE_LANDING_THRESHOLD);
        Serial.println(verticalAcceleration);
        Serial.println(launch_state);
        launch_state = 3;
        Serial.println("Landing detected!");
        eventLog.write("Landing detected");
        test = false;
        // landIndex = rows;
    }
    // todo add a condition for being high enough in the air.
    // APOGEE CONDITION
    if ((launch_state == 1 && verticalAcceleration > LAUNCH_MIN_M_PER_S) || (launch_state == 2 && test))
    {
        // todo increase precision/accuracy for these conditions
        launch_state = 2;
        output = true;
        eventLog.write("Apogee detected");
        test = false;
        // apogeeIndex = rows;
    }
    return output;
}
