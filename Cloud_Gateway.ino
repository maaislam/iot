/*
 *
 *  This example demonstrates usage of the AIR430BoostETSI library which uses
 *  the 430Boost-CC110L AIR Module BoosterPack created by Anaren Microwave, Inc.
 *  and available through the TI eStore, for the European Union.
 */





#include <AIR430BoostFCC.h>
#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>



// -----------------------------------------------------------------------------
/**
 *  Global data
 */

unsigned char rxData[60];           // Data to read from radio RX FIFO (60 bytes MAX.)
char ssid[] = "maaislam";          // LenovoWIFI your network name also called SSID
char password[] = "123asdfgh";       // your network password
char server[] = "broker.hivemq.com";  // MQTTServer to use
WiFiClient wifiClient;              // Connecting to MQTT broker via Wi-Fi
PubSubClient client(server, 1883, callback, wifiClient);  // Initialize MQTT client

// -----------------------------------------------------------------------------
// RF packet received!

void printRxData()
{
  // If RF data received, print diagnostic info to Serial Monitor & Publish over MQTT
  Serial.print("RX (DATA, RSSI, LQI, CRCBIT): ");
  Serial.print("(");
  Serial.print((char*)rxData);
  Serial.print(", ");
  Serial.print(Radio.getRssi());
  Serial.print(", ");
  Serial.print(Radio.getLqi());
  Serial.print(", ");
  Serial.print(Radio.getCrcBit());
  Serial.println(")");

  // Publish latest RF payload to the cloud via MQTT, Blink Yellow LED if successful
  if(client.publish("SensorDataSaad",(char*)rxData)) {
    digitalWrite(YELLOW_LED, HIGH);
    Serial.println("MQTT Publish success!");
    digitalWrite(YELLOW_LED, LOW);
  } else {
    Serial.println("MQTT Publish failed!");
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
// Not used
}
//
// -----------------------------------------------------------------------------
// Main

void setup()
{
  //Setup LED for example demonstration purposes.
  pinMode(RED_LED, OUTPUT);       // RED LED Notifier for subGHz RF Rx
  digitalWrite(RED_LED, LOW);
  pinMode(GREEN_LED, OUTPUT);     // GREEN LED Notifier for Wi-Fi connected
  digitalWrite(GREEN_LED, LOW);
  pinMode(YELLOW_LED, OUTPUT);    // YELLOW LED Notifier for MQTT Pub successful
  digitalWrite(YELLOW_LED, LOW);
  pinMode(PUSH1, INPUT);          // Configure PUSH1. Used to decide how we will connect to Wi-Fi

  // Setup serial for debug printing.
  Serial.begin(115200);

    Serial.print("Attempting to connect to Network named: ");
    // print the network name (SSID);
    Serial.println(ssid);
    // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
    WiFi.begin(ssid, password);
    while ( WiFi.status() != WL_CONNECTED) {
      // print dots while we wait to connect
      Serial.print(".");
      delay(300);}

  // Connected to Wi-Fi!
  Serial.println("\nYou're connected to the network");
  Serial.println("Waiting for an ip address");

  // Wait for IP Address
  while (WiFi.localIP() == INADDR_NONE) {
    // print dots while we wait for an ip addresss
    Serial.print(".");
    delay(300);
  }
  Serial.println("\nIP Address obtained");

  // We are connected and have an IP address. Print the WiFi status.
  printWifiStatus();

  // ATTEMPT TO INITIALIZE CC110L SUBGHz RADIO
  // The radio library uses the SPI library internally, this call initializes
  // SPI/CSn and GDO0 lines. Also setup initial address, channel, and TX power.
  Radio.begin(0x01, CHANNEL_1, POWER_MAX);
}

void loop()
{
  // Reconnect to MQTT Broker if the connection was lost
  if (!client.connected()) {
    Serial.println("Disconnected. Reconnecting to MQTT Broker");
    client.connect("123asdfg_gateway1");
    Serial.println("Connected to MQTT Broker!");
  }

  // Turn on the receiver and listen for incoming data. Timeout after 1 seconds.
  // The receiverOn() method returns the number of bytes copied to rxData.
  if (Radio.receiverOn(rxData, 60, 1000) > 0)
  {
    /**
     *  Data has been received and has been copied to the rxData buffer provided
     *  to the receiverOn() method. At this point, rxData is available. See
     *  printRxData() for more information.
     */
    digitalWrite(RED_LED, HIGH);
    printRxData();                  // RX debug information
    digitalWrite(RED_LED, LOW);
  }
  
  // Ping MQTT broker to maintain connection
  client.poll();
}

void printWifiStatus() {
  // print the SSID of the network you're attached to:
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());

  // print your WiFi IP address:
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);

  // print the received signal strength:

  digitalWrite(GREEN_LED, HIGH);  // Connected to WiFi LED
}
