#include <Arduino.h>
#include <WiFiNINA.h>
#include <FlowMeter.h>
#include "WiFiSettings.h"

FlowMeter *Meter;

const int PIN_RED = A0;
const int PIN_GREEN = A1;
const int PIN_BLUE = A2;

const int BUZZER_PIN = A4;
const byte SENSOR_PIN = 2;

const unsigned long period = 1000;

const int targetMilliLitres = 2000; // Set at 2 litres for now. Makes testing less wasteful.
float progressTowardsTarget;
float nextBuzz = 0.25;

int wifi_status = WL_IDLE_STATUS;

float calibrationFactor = 9.4;

void SensorISR() { Meter->count(); }
// attachInterrupt
void setup()
{
    Serial.begin(9600);
    while (!Serial);

    pinMode(SENSOR_PIN, INPUT_PULLUP);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);

    pinMode(PIN_RED, OUTPUT);
    pinMode(PIN_GREEN, OUTPUT);
    pinMode(PIN_BLUE, OUTPUT);

    analogWrite(PIN_RED, 0);
    analogWrite(PIN_GREEN, 255);
    analogWrite(PIN_BLUE, 0);

    while (wifi_status != WL_CONNECTED)
    {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(ssid);
        wifi_status = WiFi.begin(ssid, pass);
        delay(1000);
    }
    Serial.println("Connected to wifi");
    // attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), SensorISR, FALLING);
    Meter =
        new FlowMeter(digitalPinToInterrupt(SENSOR_PIN), UncalibratedSensor, SensorISR, FALLING);

}

void loop()
{
    delay(period);
    Meter->tick(period);

    digitalWrite(LED_BUILTIN, WiFi.status() == WL_CONNECTED ? HIGH : LOW);

    progressTowardsTarget = min(Meter->getTotalVolume() / targetMilliLitres, 1.0);
    analogWrite(PIN_RED, 255 * progressTowardsTarget);
    analogWrite(PIN_GREEN, 255 * (1 - progressTowardsTarget));
    if (progressTowardsTarget >= nextBuzz)
    {
        // Beep the buzzer 1-4 times, depending on the progress towards target.
        for (int i = 0; i < nextBuzz/0.25; i++)
        {
            digitalWrite(BUZZER_PIN, HIGH);
            delay(100);
            digitalWrite(BUZZER_PIN, LOW);
        }
        nextBuzz += 0.25;
    }

}