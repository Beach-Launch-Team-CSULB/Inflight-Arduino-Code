//
// Created by shawn on 3/25/22.
//

#include "Detection.h"
#include "Utilities.h"
#include <Streaming.h>

bool Detection::updateDetection() {
    bool output = false;
    ////////////////////////////////////////////
    //***INFLIGHT DETECTION***

    //<--- Get the averages for vertical acceleration
    double sum1 = 0;
    for (auto & i : accelYBuff)
    {
            sum1 += (-1 * i.acceleration[0]);
    }
    double verticalAcceleration = sum1 / accelYBuff.size();
    //---->

    //<--- Get the averages for altitude
    for (auto & i : altBuff)
    {
        sum1 += i.altitude;
    }
    double altitudeAvg1 = sum1 / altBuff.size();
    //altitudeAvg2 = sum2 / (altBuff.size() / 2.0);
    //---->
    // cout<< verticalAcceleration;
    // LAUNCH CONDITIONS
    if ((launch_state == 0 && (verticalAcceleration > 2 || verticalAcceleration < -2) &&
    accelYBuff.size() == imuBufferSize) || (launch_state == 1 && test))
    {
        Serial.println("**Launch detected***");
        Serial.println(verticalAcceleration);
        for(auto & i : accelYBuff) {
            imu->write(&i);
        }
        for(auto & i : altBuff) {
            bps->write(&i);
        }
        launch_state = 1;
        output = true;
        test = false;
        // launchIndex = rows;
    }
    if(altitudeAvg1 > ALTITUDE_LANDING_THRESHOLD && !alt_lock) {
        alt_lock = true;
    }
    // landing altitude threshold occurs at index 1190581636 | the actual value is 4.67 IN ALT_002.csv from 08/14/2021
    // the real code should check the altitude is under 5
    // //todo Remove the line and add a condition to check the (altitude < 5)
    // LAND CONDITIONS
    if (((launch_state == 1 || launch_state == 2) && (altitudeAvg1 < ALTITUDE_LANDING_THRESHOLD &&
    (verticalAcceleration > 25 || verticalAcceleration < 25 )) && alt_lock) || (launch_state == 3 && test))
    {
        // note: if apogee detection ends up working well could
        //  add that as a condition for land
        launch_state = 3;
        output = true;
        Serial.println("Test");
        Serial.println(test);
        Serial.println("AltAvg1");
        Serial.println(altitudeAvg1);
        Serial.println(ALTITUDE_LANDING_THRESHOLD);
        Serial.println(verticalAcceleration);
        Serial.println(launch_state);
        eventLog.write("Landing detected");
        test = false;
        // landIndex = rows;
    }
    // todo add a condition for being high enough in the air.
    // APOGEE CONDITION
    if ((launch_state == 1 && verticalAcceleration < 2 && verticalAcceleration < -2) || (launch_state == 2 && test))
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

void Detection::setPointers(ICM * imu_ptr, BMP * bps_ptr) {
    imu = imu_ptr;
    bps = bps_ptr;

}


void Detection::addIMUData(const icm_struct& icm_data) const {
    if (accelYBuff.size() >= imuBufferSize)
    {
        accelYBuff.pop_back();
    }
    accelYBuff.push_front(icm_data);
}

void Detection::addBPSData(const bmp_struct& bmp_data) const {
//    Serial.println("Adding BPS data");
//    Serial.println(altBuff.max_size());
//    Serial.println(altBuff.size());
//    Serial.println(bpsBufferSize);
    if (altBuff.size() >= bpsBufferSize && !altBuff.empty())
    {
//        Serial.println("Popping data");
        altBuff.pop_back();
    }
//    Serial.println("Pushing data");
    altBuff.push_front(bmp_data);
//    Serial.println("Finished adding BPS data");
}

Detection::Detection() {
    launch_state = 0;
    test = false;
    alt_lock = false;
}

void Detection::setState(uint8_t state) {
    launch_state = state;
    test = true;
}
