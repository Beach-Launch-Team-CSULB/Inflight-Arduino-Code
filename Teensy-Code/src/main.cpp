#include <Adafruit_BMP3XX.h>
#include <Adafruit_ICM20649.h>
#include <Adafruit_BNO08x.h>
#include <InternalTemperature.h>
#include <SD.h>
#include <TimeLib.h>
#include <TinyGPS++.h>
#include <LittleFS.h>

#define cm + "," +
#define GPSSerial Serial8

const uint_fast8_t BMP_CS = 15, ICM_CS = 16, XTSD_CS = 36, BNO_INT = 21, BNO_CS = 3, BNO_RESET = 19;
const uint_fast16_t GPS_BAUD = 9600;
const float sea_level_pressure_hpa = 1016;
LittleFS_QSPIFlash flash;

bool bmp_enabled, bno_enabled, icm_enabled, xtsd_enabled, teensy_sd_enabled, gps_enabled, flash_enabled, time_set;
String reading_time;
uint_fast32_t counter, start_time, log_time;
int_fast32_t gps_time, gps_speed, gps_date, gps_year, gps_month, gps_day, gps_hour,
        gps_minute, gps_second, gps_centisecond;
double gps_altitude, gps_latitude, gps_longitude, gps_hdop, gps_course_deg, gps_reading_age;
float bno_accel[3], bno_mag[3], bno_gyro[3];
Adafruit_ICM20649 icm;
Adafruit_BMP3XX bmp;
Adafruit_BNO08x bno(BNO_RESET);
TinyGPSPlus gps;
SDClass teensy_sd, xtsd;
String sd_string = "output0.csv";
String xtsd_string = "output0.csv";
String flash_string = "output0.csv";

void setup() {
    bno_enabled = false;
    gps_enabled = false;
    time_set = false;
    GPSSerial.begin(9600);
    Serial.print("Start logging\n");
    bmp_enabled = bmp.begin_SPI(BMP_CS);
    if (!bmp_enabled) {
        Serial.print("BMP Init failed!\n");
    } else {
        Serial.print("BMP Initialized!\n");
        bmp.setOutputDataRate(BMP3_ODR_200_HZ);
        bmp.setPressureOversampling(BMP3_OVERSAMPLING_32X);
        bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_32X);
        bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_7);
    }
    icm_enabled = icm.begin_SPI(ICM_CS);
    if (!icm_enabled) {
        Serial.print("ICM Init failed!\n");
    } else {
        Serial.print("ICM Initialized!\n");
        icm.setAccelRange(ICM20649_ACCEL_RANGE_30_G);
        icm.setGyroRange(ICM20649_GYRO_RANGE_4000_DPS);
        icm.setAccelRateDivisor(ICM20X_ACCEL_FREQ_473_HZ);
        icm.setGyroRateDivisor(ICM20X_GYRO_FREQ_361_4_HZ);
    }
    bno_enabled = bno.begin_SPI(BNO_CS, BNO_INT);
    if(!bno_enabled) {
        Serial.print("BNO Init failed!\n");
    }
    else {
        Serial.print("BNO Initialized!\n");
        bno.enableReport(SH2_ACCELEROMETER);
        bno.enableReport(SH2_GYROSCOPE_CALIBRATED);
        bno.enableReport(SH2_MAGNETIC_FIELD_CALIBRATED);
    }
    teensy_sd_enabled = teensy_sd.begin(BUILTIN_SDCARD);
    if (!teensy_sd_enabled) {
        Serial.print("Teensy SD Init failed!\n");
    } else {
        Serial.print("Teensy SD Initialized!\n");
    }
    xtsd_enabled = xtsd.begin(XTSD_CS);
    if (!xtsd_enabled) {
        Serial.print("XTSD Init failed!\n");
    } else {
        Serial.print("XTSD Initialized!\n");
        if (teensy_sd_enabled) {
            if (!teensy_sd.exists("/xtsd/")) {
                teensy_sd.mkdir("/xtsd/");
            }
            Serial.println("Starting Data Dump from XTSD Module");
            uint_fast32_t count = 0;
            String file_name = "/xtsd/xtsd_dump0/";
            while (teensy_sd.exists(file_name.c_str())) {
                count++;
                file_name = "/xtsd/xtsd_dump" + String(count) + "/";
            }
            teensy_sd.mkdir(file_name.c_str());
            String xtsd_file = "output0.csv";
            uint_fast16_t xtsd_count = 0;
            uint_fast16_t dump_num = 0;
            for (uint_fast16_t i = 0; i < 256; i++) {
                if (xtsd.exists(xtsd_file.c_str())) {
                    Serial.print("Dumping ");
                    Serial.println(xtsd_file.c_str());
                    dump_num++;
                    String xtsd_path = file_name + xtsd_file;
                    File teensy_copy = teensy_sd.open(xtsd_path.c_str(), FILE_WRITE);
                    File xtsd_copy = xtsd.open(xtsd_file.c_str(), FILE_READ);
                    size_t n;
                    auto *buf = new uint_fast8_t[128];
                    while ((n = xtsd_copy.read(buf, sizeof(buf))) > 0) {
                        teensy_copy.write(buf, n);
                    }
                    delete[] buf;
                    Serial.print("Dumping " + String(xtsd_file.c_str()) + " Finished");
                    teensy_copy.close();
                    xtsd_copy.close();
                    xtsd.remove(xtsd_path.c_str());
                }
                xtsd_count++;
                xtsd_file = "output" + String(xtsd_count) + ".csv";
            }
            Serial.println("Copy from XTSD finished");
            Serial.println(String(dump_num) + " Files copied");
        }
    }
    flash_enabled = flash.begin();
    if (!flash_enabled) {
        Serial.println("Flash chip not initialized!");
    } else {
        Serial.println("Flash chip initialized!");
        if (teensy_sd_enabled) {
            if (!teensy_sd.exists("/flash/")) {
                teensy_sd.mkdir("/flash/");
            }
            Serial.println("Starting Data Dump from Flash chip");
            uint_fast16_t count = 0;
            String file_name = "/flash/flash_dump0/";
            while (teensy_sd.exists(file_name.c_str())) {
                count++;
                file_name = "/flash/flash_dump" + String(count) + "/";
            }
            teensy_sd.mkdir(file_name.c_str());
            String flash_file = "output0.csv";
            uint_fast16_t flash_count = 0;
            uint_fast16_t dump_num = 0;
            for (uint_fast16_t i = 0; i < 256; i++) {
                if (xtsd.exists(flash_file.c_str())) {
                    dump_num++;
                    Serial.print("Dumping " + String(flash_file.c_str()));
                    String flash_path = file_name + flash_file;
                    File teensy_copy = teensy_sd.open(flash_path.c_str(), FILE_WRITE);
                    File flash_copy = xtsd.open(flash_file.c_str(), FILE_READ);
                    size_t n;
                    auto *buf = new uint_fast8_t[128];
                    while ((n = flash_copy.read(buf, sizeof(buf))) > 0) {
                        teensy_copy.write(buf, n);
                    }
                    delete[] buf;
                    Serial.print("Dumping " + String(flash_file.c_str()) + " Finished");
                    teensy_copy.close();
                    flash_copy.close();
                    flash.remove(flash_path.c_str());
                }
                flash_count++;
                flash_file = "output" + String(flash_count) + ".csv";
            }
            Serial.println("Copy from flash finished");
            Serial.println(String(dump_num) + " Files copied");
        }
    }
    if (GPSSerial.available() > 0) {
        Serial.print("GPS Connected!\n");
        gps_enabled = true;
    }
    uint_fast32_t count = 0;
    if (teensy_sd_enabled) {
        while (teensy_sd.exists(sd_string.c_str())) {
            count++;
            sd_string = "output" + String(count) + ".csv";
        }
        Serial.print("SD writing to: ");
        Serial.println(sd_string);
    }
    if (xtsd_enabled) {
        while (xtsd.exists(xtsd_string.c_str())) {
            count++;
            xtsd_string = "output" + String(count) + ".csv";
        }
    }
    if (flash_enabled) {
        while (flash.exists(flash_string.c_str())) {
            count++;
            flash_string = "output" + String(count) + ".csv";
        }
    }
    String file_hdr = "Current Time, Time Since Startup, Teeny Internal Temperature";
    file_hdr.reserve(1024);

    if (icm_enabled) {
        file_hdr += ", ICM Temperature, ICM Gyro X, ICM Gyro Y, "
                    "ICM Gyro Z, ICM Gyro Velocity X, ICM Gyro Velocity Y, ICM Gyro Velocity Z,"
                    "ICM Gyro Heading, ICM Gyro Pitch, ICM Gyro Roll, ICM Accel X, ICM Accel Y, ICM Accel Z, "
                    "ICM Accel Velocity X, ICM Accel Velocity Y, ICM Accel Velocity Z, ICM Accel Heading, "
                    "ICM Accel Pitch, ICM Accel Roll";
    }
    if (bmp_enabled) {
        file_hdr += ", BMP Temperature, BMP Pressure, BMP Altitude";
    }
    if (bno_enabled) {
        file_hdr += ", BNO Accel X, BNO Accel Y, BNO Accel Z, BNO Gyro X, BNO Gyro Y, BNO Gyro Z, BNO Mag X, "
                    "BNO Mag Y, BNO Mag Z";
    }
    if (gps_enabled) {
        file_hdr += ", GPS Altitude, GPS Year, GPS Month, GPS Day, GPS Hour, GPS Minute, GPS Second, GPS Centisecond,"
                    "GPS Latitude, GPS Longitude, GPS Speed, GPS HDOP, GPS Course Degrees,"
                    "GPS Reading Age";
    }

    file_hdr += "\n";
    Serial.print(file_hdr);
    if (teensy_sd_enabled) {
        File teensy_file = teensy_sd.open(sd_string.c_str(), FILE_WRITE);
        teensy_file.print(file_hdr);
        teensy_file.close();
    }
    if(xtsd_enabled) {
        File xtsd_file = xtsd.open(xtsd_string.c_str(), FILE_WRITE);
        xtsd_file.print(file_hdr);
        xtsd_file.close();
    }
    if(flash_enabled) {
        File flash_file = flash.open(flash_string.c_str(), FILE_WRITE);
        flash_file.print(file_hdr);
        flash_file.close();
    }
}

void loop() {
    float teensy_temp = InternalTemperatureClass::readTemperatureC();
    float bmp_temp, bmp_pressure, bmp_altitude;
    if (bmp_enabled) {
        bmp.performReading();
        bmp_temp = bmp.readTemperature();
        bmp_pressure = bmp.readPressure();
        bmp_altitude = bmp.readAltitude(sea_level_pressure_hpa);
    }
    sensors_event_t icm_accel, icm_gyro, icm_temp;
    if (icm_enabled) {
        icm.getEvent(&icm_accel, &icm_gyro, &icm_temp);
    }
    if (bno_enabled) {
        sh2_SensorValue sensor{};
        bool found_accel = false, found_gyro = false, found_mag = false;
        bno.getSensorEvent(&sensor);
        while(!found_accel || !found_mag || !found_gyro) {
            switch (sensor.sensorId) {
                case SENSOR_TYPE_ACCELEROMETER:
                    found_accel = true;
                    bno_accel[0] = sensor.un.accelerometer.x;
                    bno_accel[1] = sensor.un.accelerometer.y;
                    bno_accel[2] = sensor.un.accelerometer.z;
                    break;
                case SENSOR_TYPE_MAGNETIC_FIELD:
                    found_mag = true;
                    bno_mag[0] = sensor.un.magneticField.x;
                    bno_mag[1] = sensor.un.magneticField.y;
                    bno_mag[2] = sensor.un.magneticField.z;
                    break;
                case SENSOR_TYPE_GYROSCOPE:
                    found_gyro = true;
                    bno_gyro[0] = sensor.un.gyroscope.x;
                    bno_gyro[1] = sensor.un.gyroscope.y;
                    bno_gyro[2] = sensor.un.gyroscope.z;
                    break;
                default:
                    Serial.print("Sensor type " + String(sensor.sensorId) + " Not being used");
                    break;
            }
        }
    }
    while (GPSSerial.available()) {
        gps_enabled = true;
        gps.encode(GPSSerial.read());
    }
    if (gps_enabled) {
        gps_altitude = gps.altitude.meters();
        gps_year = gps.date.year();
        gps_month = gps.date.month();
        gps_day = gps.date.day();
        gps_hour = gps.time.hour();
        gps_minute = gps.time.minute();
        gps_second = gps.time.second();
        gps_centisecond = gps.time.centisecond();
        gps_latitude = gps.location.lat();
        gps_longitude = gps.location.lng();
        gps_speed = gps.speed.value();
        gps_hdop = gps.hdop.hdop();
        gps_course_deg = gps.course.deg();
        gps_reading_age = gps.time.age();
        if (gps.time.age() < 50 && !time_set) {
            setTime(gps_hour, gps_minute, gps_second, gps_day, gps_month, gps_year);
            time_set = true;
            Serial.println("Clock synchronized with GPS time");
        }
    }
    log_time = now() + 5000;
    reading_time = String(now());
    String write_str = reading_time cm millis() cm teensy_temp;
    write_str.reserve(4096);
    if (icm_enabled) {
        write_str += "," + String(icm_temp.temperature) cm icm_gyro.gyro.x cm icm_gyro.gyro.y cm
                     icm_gyro.gyro.z cm icm_gyro.gyro.v[0] cm
                     icm_gyro.gyro.v[1] cm icm_gyro.gyro.v[2] cm icm_gyro.gyro.heading cm
                     icm_gyro.gyro.pitch cm icm_gyro.gyro.roll cm icm_accel.acceleration.x cm
                     icm_accel.acceleration.y cm icm_accel.acceleration.z cm
                     icm_accel.acceleration.v[0] cm icm_accel.acceleration.v[1] cm
                     icm_accel.acceleration.v[2] cm icm_accel.acceleration.heading cm
                     icm_accel.acceleration.pitch cm icm_accel.acceleration.roll;
    }
    if (bmp_enabled) {
        write_str += "," + String(bmp_temp) cm String(bmp_pressure) cm
                     String(bmp_altitude);
    }
    if(bno_enabled) {
        write_str += "," + String(bno_accel[0]) cm bno_accel[1] cm bno_accel[2] cm bno_gyro[0] cm bno_gyro[1]
                cm bno_gyro[2] cm bno_mag[0] cm bno_mag[1] cm bno_mag[2];
    }
    if (gps_enabled) {
        write_str += "," + String(gps_altitude) cm gps_year cm gps_month cm gps_day cm gps_hour cm gps_minute
                     cm gps_second cm gps_centisecond
                     cm gps_latitude cm gps_longitude cm gps_speed cm gps_hdop
                     cm gps_course_deg cm gps_reading_age;
    }
    write_str.append("\n");
    if (teensy_sd_enabled) {
        File teensy_file = teensy_sd.open(sd_string.c_str(), FILE_WRITE);
        teensy_file.print(write_str);
        teensy_file.close();
    }
    if (xtsd_enabled) {
        File xtsd_file = xtsd.open(xtsd_string.c_str(), FILE_WRITE);
        xtsd_file.print(write_str);
        xtsd_file.close();
    }
    if (flash_enabled)
        if (Serial) {
            Serial.print(write_str);
        }
    // delay(1000);
}