#ifndef PIR_SENSOR_H
#define PIR_SENSOR_H

#include <Arduino.h>

class PIRSensor {
public:
    PIRSensor(int pin);
    void begin();
    static void handleMotion();
    static volatile int motionDetected; 
private:
    static int pirPin;
};

#endif
