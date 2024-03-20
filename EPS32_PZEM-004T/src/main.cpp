#include <Arduino.h>
#include <PZEM004Tv30.h>
#include <WiFi.h>
#include <PubSubClient.h>

// WiFi
const char *ssid = "BBB"; // Enter your Wi-Fi name
const char *password = "Best300436";  // Enter Wi-Fi password

// MQTT Broker
const char *mqtt_broker = "broker.hivemq.com";
const char *voltagetopic = "pzem/voltage";
const char *currenttopic = "pzem/current";
const char *powertopic = "pzem/power";
const char *energytopic = "pzem/energy";
const char *frequencytopic = "pzem/frequency";
const char *pftopic = "pzem/pf";
const char *mqtt_username = "";
const char *mqtt_password = "";
const int mqtt_port = 1883;
const int mqtt_port = 1900;

WiFiClient espClient;
PubSubClient client(espClient);

#if !defined(PZEM_RX_PIN) && !defined(PZEM_TX_PIN)
#define PZEM_RX_PIN 16
#define PZEM_TX_PIN 17
#endif

#if !defined(PZEM_SERIAL)
#define PZEM_SERIAL Serial2
#endif


#if defined(ESP32)
/*************************
 *  ESP32 initialization
 * ---------------------
 * 
 * The ESP32 HW Serial interface can be routed to any GPIO pin 
 * Here we initialize the PZEM on Serial2 with RX/TX pins 16 and 17
 */
PZEM004Tv30 pzem(PZEM_SERIAL, PZEM_RX_PIN, PZEM_TX_PIN);
#elif defined(ESP8266)
/*************************
 *  ESP8266 initialization
 * ---------------------
 * 
 * Not all Arduino boards come with multiple HW Serial ports.
 * Serial2 is for example available on the Arduino MEGA 2560 but not Arduino Uno!
 * The ESP32 HW Serial interface can be routed to any GPIO pin 
 * Here we initialize the PZEM on Serial2 with default pins
 */
//PZEM004Tv30 pzem(Serial1);
#else
/*************************
 *  Arduino initialization
 * ---------------------
 * 
 * Not all Arduino boards come with multiple HW Serial ports.
 * Serial2 is for example available on the Arduino MEGA 2560 but not Arduino Uno!
 * The ESP32 HW Serial interface can be routed to any GPIO pin 
 * Here we initialize the PZEM on Serial2 with default pins
 */



PZEM004Tv30 pzem(PZEM_SERIAL);
#endif


void callback(char *topic, byte *payload, unsigned int length) {
    Serial.print("Message arrived in topic: ");
    Serial.println(voltagetopic);
    Serial.print("Message:");
    Serial.print("Message arrived in topic: ");
    Serial.println(currenttopic);
    Serial.print("Message:");
    Serial.print("Message arrived in topic: ");
    Serial.println(powertopic);
    Serial.print("Message:");
    Serial.print("Message arrived in topic: ");
    Serial.println(energytopic);
    Serial.print("Message:");
    Serial.print("Message arrived in topic: ");
    Serial.println(frequencytopic);
    Serial.print("Message:");
    Serial.println(pftopic);
    Serial.print("Message:");
    Serial.print("Message arrived in topic: ");
    for (int i = 0; i < length; i++) {
        Serial.print((char) payload[i]);
    }
    Serial.println();
    Serial.println("-----------------------");
}

void setup() {
     // Set software serial baud to 115200;
    Serial.begin(115200);
    // Connecting to a WiFi network
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.println("Connecting to WiFi..");
    }
    Serial.println("Connected to the Wi-Fi network");
    //connecting to a mqtt broker
    client.setServer(mqtt_broker, mqtt_port);
    client.setCallback(callback);
    while (!client.connected()) {
        String client_id = "esp32-client-";
        client_id += String(WiFi.macAddress());
        Serial.printf("The client %s connects to the public MQTT broker\n", client_id.c_str());
        if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
            Serial.println("Public EMQX MQTT broker connected");
        } else {
            Serial.print("failed with state ");
            Serial.print(client.state());
            delay(2000);
        }
    }

}

void loop() {
    
    client.loop();

    // Print the custom address of the PZEM
    Serial.print("Custom Address:");
    Serial.println(pzem.readAddress(), HEX);

    // Read the data from the sensor
    float voltage = pzem.voltage();
    float current = pzem.current();
    float power = pzem.power();
    float energy = pzem.energy();
    float frequency = pzem.frequency();
    float pf = pzem.pf();

    // Check if the data is valid
    if(isnan(voltage)){
        Serial.println("Error reading voltage");
    } else if (isnan(current)) {
        Serial.println("Error reading current");
    } else if (isnan(power)) {
        Serial.println("Error reading power");
    } else if (isnan(energy)) {
        Serial.println("Error reading energy");
    } else if (isnan(frequency)) {
        Serial.println("Error reading frequency");
    } else if (isnan(pf)) {
        Serial.println("Error reading power factor");
    } else {

        // Print the values to the Serial console
        Serial.print("Voltage: ");      Serial.print(voltage);      Serial.println("V");
        Serial.print("Current: ");      Serial.print(current);      Serial.println("A");
        Serial.print("Power: ");        Serial.print(power);        Serial.println("W");
        Serial.print("Energy: ");       Serial.print(energy,3);     Serial.println("kWh");
        Serial.print("Frequency: ");    Serial.print(frequency, 1); Serial.println("Hz");
        Serial.print("PF: ");           Serial.println(pf);

    }

  // Publish and subscribe

    char voltageStr[10];
    snprintf(voltageStr, sizeof(voltageStr), "%.2f", voltage);
    client.publish(voltagetopic, voltageStr);
    client.subscribe(voltagetopic);

    char currentStr[10];
    snprintf(currentStr, sizeof(currentStr), "%.2f", current);
    client.publish(currenttopic, currentStr);
    client.subscribe(currenttopic);

    char powerStr[10];
    snprintf(powerStr, sizeof(powerStr), "%.2f", power);
    client.publish(powertopic, powerStr);
    client.subscribe(powertopic);

    char energyStr[10];
    snprintf(energyStr, sizeof(energyStr), "%.2f", energy);
    client.publish(energytopic, energyStr);
    client.subscribe(energytopic);

    char frequencyStr[10];
    snprintf(frequencyStr, sizeof(frequencyStr), "%.2f", frequency);
    client.publish(frequencytopic, frequencyStr);
    client.subscribe(frequencytopic);

    char pfStr[10];
    snprintf(pfStr, sizeof(pfStr), "%.2f", pf);
    client.publish(pftopic, pfStr);
    client.subscribe(pftopic);

    Serial.println();
    delay(2000);
}