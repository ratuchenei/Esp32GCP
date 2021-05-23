/******************************************************************************
 * Copyright 2018 Google
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *****************************************************************************/

#if defined(ARDUINO_SAMD_MKR1000) or defined(ESP8266)
#define __SKIP_ESP32__
#endif

#if defined(ESP32)
#define __ESP32_MQTT_H__
#endif

#ifdef __SKIP_ESP32__

#include <Arduino.h>

void setup(){
  Serial.begin(115200);
}

void loop(){
  Serial.println("Init");
}

#endif

#ifdef __ESP32_MQTT_H__
#include "esp32-mqtt.h"

#include <DHT.h>
#include <DHT_U.h>
#include <ArduinoJson.h>


#define DHTPIN 4     
#define DHTTYPE DHT11  

DHT_Unified dht(DHTPIN, DHTTYPE);

char buffer [100];

void setup() 
{    
    Serial.begin(115200);
 
    dht.begin();
    Serial.println(F("DHT11 Unified Sensor"));
 
    sensor_t sensor;
    dht.temperature().getSensor(&sensor);
  
    Serial.println(F("------------------------------------"));
    Serial.println(F("Temperature Sensor"));
    Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
    Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
    Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
    Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
    Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
    Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
    Serial.println(F("------------------------------------"));

    dht.humidity().getSensor(&sensor);

    Serial.println(F("Humidity Sensor"));
    Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
    Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
    Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
    Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
    Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
    Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
    Serial.println(F("------------------------------------"));
    
    
    pinMode(LED_BUILTIN, OUTPUT);
    setupCloudIoT();
}

unsigned long lastMillis = 0;

void loop() 
{
    mqtt->loop();
    
    delay(10);  // <- fixes some issues with WiFi stability
  
    if (!mqttClient->connected()) 
    {
        connect();
    }
    
    sensors_event_t event;
    
    // TODO: replace with your code
    // publish a message roughly every second.
    
    if (millis() - lastMillis > 5000) 
    {
        lastMillis = millis();

        StaticJsonDocument <100> doc;
        dht.temperature().getEvent(&event);     
        //Serial.print(F("Temperature: "));
        //Serial.print(event.temperature);
        //Serial.println(F("°C"));
        doc ["temperature"] = event.temperature;

        dht.humidity().getEvent(&event);
        //Serial.print(F("Humidity: "));
        //Serial.print(event.relative_humidity);
        //Serial.println(F("%"));
        doc ["humidity"] = event.relative_humidity;
        
        doc ["timestamp"] = time(nullptr);        

        serializeJson (doc, buffer);
                
        Serial.println(buffer);
        
        //publishTelemetry(mqttClient, "/sensors", buffer);
        
        publishTelemetry(buffer);
    }
}
#endif
