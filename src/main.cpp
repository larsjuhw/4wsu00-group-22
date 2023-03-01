#include <Arduino.h>
#include <FlowMeter.h>

FlowMeter *Meter;
const byte sensorPin = 2;
const unsigned long delayPeriod = 1000;

void SensorISR() { Meter->count(); }
// attachInterrupt
void setup()
{
    Serial.begin(9600);
    Meter =
        new FlowMeter(digitalPinToInterrupt(sensorPin), UncalibratedSensor, SensorISR, RISING);
}

void loop()
{
    delay(delayPeriod);

    Meter->tick(delayPeriod);
    Serial.println("Flow rate: " + String(Meter->getCurrentFlowrate()) + " l/min");
    Serial.println("Total flow: " + String(Meter->getTotalVolume()) + " l");
}