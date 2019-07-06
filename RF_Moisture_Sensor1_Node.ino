
// The AIR430BoostFCC library uses the SPI library internally. Energia does not
// copy the library to the output folder unless it is referenced here.
// The order of includes is also important due to this fact.

#include <SPI.h>
#include <AIR430BoostFCC.h>
#include <aJSON.h>
#include <LCD_Launchpad.h>
#include <Filter.h>



// -----------------------------------------------------------------------------
/**
 *  Global data
 */
const char analogPin = A7;



const char* mySensor = "M1";//M1 = Moisture Sensor 1 in short






LCD_LAUNCHPAD myLCD;

// -----------------------------------------------------------------------------
// Main

// Simple function for scrolling messages across the on-board LCD
void scrollText(String text)
{
  Serial.println("System Initialising");
  myLCD.displayText(text);
  Serial.println(text);
  delay(400);
  while(text != ""){
    myLCD.clear();
    text = text.substring(1);
    Serial.println(text);
    myLCD.displayText(text);
    delay(175);
  }
}

// SETUP function for sketch
void setup()
{
  // Setup serial for debug printing.
  Serial.begin(9600);
  Serial.println("\n");




  // The radio library uses the SPI library internally, this call initializes
  // SPI/CSn and GDO0 lines. Also setup initial address, channel, and TX power.
  Radio.begin(0x01, CHANNEL_1, POWER_MAX);

  // Configure the on-board LCD of the MSP430FR6989 LaunchPad
  myLCD.init();

  // Print welcome messages to the LCD screen
  //String welcome = "Hello "+String(myName);
  //scrollText(welcome);

  // Print sensor type to the LCD screen
  String sensorType = String("Moisture Sensor1");
  scrollText(sensorType);

  // Configure RED_LED, which will be used for visual notification of RF TX
  pinMode(GREEN_LED, OUTPUT);
  digitalWrite(GREEN_LED, LOW);   // set the LED off
  Serial.println("Setup Complete. Starting Radio");
}

// LOOP function for sketch
void loop()
{
  //Blink LED & TX segment on LCD to signify start of new sensor reading + RF TX
  digitalWrite(GREEN_LED, HIGH);
  myLCD.showSymbol(LCD_SEG_TX, 1);

  //***Exponential Filtering.*** Initialise all the readings to 0:

 ExponentialFilter<float>FilteredData(40, 0);
 int RawData = analogRead(analogPin);
  FilteredData.Filter(RawData);
  int SmoothData = FilteredData.Current();





  // Encode sensor readings into JSON
  /*Usual Desired JSON encoded format:
  {
    "d":{
    "d":{
      "Name":"MOhd",
      "Sensor":"Moisture",
      "Data": 1234
    }
  }
  but in this case I avoided this format as only 60 byte max can be transferred over RF.
  Therefore I have available space for addition sensor data.We can always add more detail to
  the Raw data when in cloud.
  */

  aJsonObject *msg = aJson.createObject();
    //aJsonObject *d = aJson.createObject();
    //aJson.addItemToObject(msg, "d", d);

    aJson.addStringToObject(msg, "Arafat", mySensor);//s represents sensor in short
    //int sensorValue = average;
    aJson.addNumberToObject(msg, "D",  SmoothData);// D2 represents data from moisture sensor 2 in short.

  // Typecast JSON message to char array, delete JSON object, then send via via RF
  char* payload = aJson.print(msg);
  aJson.deleteItem(msg);
  Radio.transmit(ADDRESS_BROADCAST, (uint8_t*)payload, 60);

  // Print latest sensor readings to LCD
  myLCD.displayText("    ");
  myLCD.displayText(String( SmoothData));

  // Print JSON-encoded payload to terminal, then free char array
  Serial.print("TX (DATA): ");
  Serial.println(payload);
  free(payload);

  // Transmission success! Toggle off LED & clear TX segment on LCD
  digitalWrite(GREEN_LED, LOW);
  myLCD.showSymbol(LCD_SEG_TX, 0);

  // Go to LPM3 for 1 second
  //sleepSeconds(1);
  delay(250);
}
