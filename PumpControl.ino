


#include <SPI.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <aJSON.h>


// -----------------------------------------------------------------------------
/**
 *  Global data
*/


const char* myName = "Arafat";
const char* myActuator = "Pump";
const char pumpStatus = 19;
const char pumpControl = 18;
//const char pumpRelay = 5;
const char pumpRelay1 = 17;

char ssid[] = "maaislam";          // LenovoWIFI your network name also called SSID
char password[] = "123asdfgh";       // your network password
char server[] = "iot.eclipse.org";  // MQTTServer to use


WiFiClient wifiClient;              // Connecting to MQTT broker via Wi-Fi

PubSubClient client(server, 1883, callback, wifiClient);  // Initialize MQTT client



void callback(char* topic, byte* payload, unsigned int length) {
    if ((char)payload[0] == '1') {
                    //digitalWrite(pumpRelay, HIGH);
                    digitalWrite(pumpRelay1, HIGH);// Turn the Relay on (Note that LOW is the voltage level
                    digitalWrite(pumpControl, HIGH);

                  } else {
                      //digitalWrite(pumpRelay,LOW);  // Turn the Relay off by making the voltage HIGH
                    digitalWrite(pumpControl, LOW);
                    digitalWrite(pumpRelay1, LOW);
                                      }
     Serial.print("Message arrived in topic: ");
       Serial.println(topic);
delay(1000);
       Serial.print("Message:");
       for (int i = 0; i < length; i++) {
         Serial.println((char)payload[i]);
       }

       Serial.println();
       // Switch on the LED if an 1 was received as first character



}
//
// -----------------------------------------------------------------------------
// Main

void setup()
{

    pinMode(pumpRelay1, OUTPUT);
     digitalWrite(pumpRelay1, LOW);

  pinMode(pumpStatus, INPUT);
  digitalWrite(pumpStatus, LOW);

  pinMode(pumpControl, OUTPUT);
  digitalWrite(pumpControl, LOW);

  //pinMode(pumpRelay, OUTPUT);
  //digitalWrite(pumpRelay, LOW);

  pinMode(RED_LED, OUTPUT);     // RED LED Notifier for MQTT Receiption
  digitalWrite(RED_LED, LOW);

  pinMode(GREEN_LED, OUTPUT);     // GREEN LED Notifier for Wi-Fi connected
  digitalWrite(GREEN_LED, LOW);

  pinMode(YELLOW_LED, OUTPUT);    // YELLOW LED Notifier for MQTT Pub successful
  digitalWrite(YELLOW_LED, LOW);


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




}

void loop()
{
  // Reconnect to MQTT Broker if the connection was lost
  if (!client.connected()) {
    Serial.println("Disconnected. Reconnecting to MQTT Broker");
    client.connect("123asdfg_gateway2");
    Serial.println("Connected to MQTT Broker!");

    //MQTT Subscribe
    client.subscribe("maaislamPumpControl");
  }


  // Encode Switch status into JSON

  /* JSON encoded format:
    {
      "d":{
      "d":{
        "Name":"MOhd",
        "Sensor":"Moisture",
        "Data": 1234
      }
    }*/
                   aJsonObject *msg = aJson.createObject();
                   aJsonObject *d = aJson.createObject();
                   aJson.addItemToObject(msg, "d", d);
                   aJson.addStringToObject(d, "Name", myName);
                   aJson.addStringToObject(d, "Actuator", myActuator);
                   int switchState = digitalRead(pumpStatus);
                   aJson.addNumberToObject(d, "Data", switchState);

   // Typecast JSON message to char array and delete JSON object
                         char* payload = aJson.print(msg);
                         aJson.deleteItem(msg);

//MQTT Publish
        client.publish("maaislamPumpStatus",(char*)payload);
        digitalWrite(YELLOW_LED, HIGH);
        Serial.println("MQTT Publish success!");
        digitalWrite(YELLOW_LED, LOW);

//Serial print payload
        Serial.println(payload);
        free(payload);




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
  //delay(1000);
}
