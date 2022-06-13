//
// Created by shawn on 3/25/22.
//

#include "Detection.h"
#include "EventLog.h"
#include <Arduino.h>
#include <Streaming.h>

Detection::Detection() {
//Constructor for the Detection class
//Initializes the launch state to 0 and assumes the system is not in test mode until set otherwise
    launch_state = 0;
    test = false;
}

void Detection::setState(uint8_t state) {
    //Used for testing purposes
    // Allows the tester to set the system state over serial

    launch_state = state;
    test = true;
}

bool Detection::updateDetection(std::deque<icm_struct> *icm_data, std::deque<bmp_struct> *bmp_data) {
    //Updates what the detection system does with new data
    //Returns true when there is a state change
    bool output = false;
    //<--- Get the averages for vertical acceleration
    double sum1 = 0;
    // Number of buffer values to use for calculation
    const uint8_t icm_data_size = 64;
    const uint8_t bmp_data_size = 16;
    // Iterates through the lesser of either the first icm_data_size values or all of the values in the deque and averages them
    for(auto i = icm_data->rbegin(); distance(icm_data->rbegin(), i) < icm_data_size && i != icm_data->rend(); ++i) {
        sum1 += i->acceleration[1];
    }
    Serial.println("Finished iteration");
    double verticalAcceleration = sum1 / icm_data_size;
    //---->
    //<--- Get the averages for altitude
    // Iterates through the lesser of either the first bmp_data_size values or all of the values in the deque and averages them
    sum1 = 0;
    for(auto i = bmp_data->rbegin(); distance(bmp_data->rbegin(), i) < bmp_data_size && i != bmp_data->rend(); ++i) {
        sum1 += i->altitude;
        Serial.println(i->toString());
        Serial.flush();
    }
    double altitudeAvg1 = sum1 / bmp_data_size;
    //altitudeAvg2 = sum2 / (altBuff.size() / 2.0);
    //---->
    // cout<< verticalAcceleration;
    // LAUNCH CONDITIONS
    // Checks if either the launch state is pre-launch, the vertical acceleration of the rocket is over the threshold,
    // at least packet_size / 2 elements are in the buffer used for IMU data to avoid fluke reads, the altitude is over
    // the landing threshold, or if the launch state has been manually set to 1 and test mode is enabled
    if ((launch_state == 0 && (abs(verticalAcceleration) > LAUNCH_MIN_M_PER_S) &&
         icm_data->size() >= packet_size / 2 && altitudeAvg1 > ALTITUDE_LANDING_THRESHOLD) || (launch_state == 1 && test))
    {
        Serial.println("**Launch detected***");
        Serial.println(verticalAcceleration);
        // Sets the launch state machine to 1 for launch pre-apogee
        launch_state = 1;
        // Returns true to indicate a state change
        output = true;
        // Disables test mode to avoid re-enabling setting
        test = false;
        // launchIndex = rows;
    }
    // landing altitude threshold occurs at index 1190581636 | the actual value is 4.67 IN ALT_002.csv from 08/14/2021
    // the real code should check the altitude is under 5
    // //todo Remove the line and add a condition to check the (altitude < 5)
    // LAND CONDITIONS
    // Checks if the launch state is launch pre-apogee or post-apogee and that the altitude is less than the threshold,
    // the vertical acceleration is under 25 m/s/s
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
        // Sets launch state to 3 for landed
        launch_state = 3;
        Serial.println("Landing detected!");
        // Writes event to event log
        eventLog.write("Landing detected");
        test = false;
        // landIndex = rows;
    }
    // todo add a condition for being high enough in the air.
    // APOGEE CONDITION
    // Checks if the launch state is launch pre-apogee and the vertical acceleration is less than the threshold needed to launch
    if ((launch_state == 1 && abs(verticalAcceleration) < LAUNCH_MIN_M_PER_S) || (launch_state == 2 && test))
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
