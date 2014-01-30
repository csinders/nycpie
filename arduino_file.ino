 /*
  Cosm sensor client
 
 This sketch connects an analog sensor to Cosm (http://www.cosm.com)
 using a Wiznet Ethernet shield. You can use the Arduino Ethernet shield, or
 the Adafruit Ethernet shield, either one will work, as long as it's got
 a Wiznet Ethernet module on board.
 
 This example has been updated to use version 2.0 of the Cosm.com API. 
 To make it work, create a feed with a datastream, and give it the ID
 sensor1. Or change the code below to match your feed.
 
 
 Circuit:
 * Analog sensor attached to analog in 0
 * Ethernet shield attached to pins 10, 11, 12, 13
 
 created 15 March 2010
 updated 16 Mar 2012
 by Tom Igoe with input from Usman Haque and Joe Saavedra
 
http://arduino.cc/en/Tutorial/PachubeClient
 This code is in the public domain.
 
 */

#include <SPI.h>
#include <Ethernet.h>

#define APIKEY         "....." // your cosm api key
#define FEEDID         ..... // your feed ID
#define USERAGENT      "Cosm Arduino Example (.....)" // user agent is the project name

// assign a MAC address for the ethernet controller.
// Newer Ethernet shields have a MAC address printed on a sticker on the shield
// fill in your address here:
byte mac[] = { 0x90, 0xA2, 0xDA, 0x00, 0x8E, 0x14};

// fill in an available IP address on your network here,
// for manual configuration:
IPAddress ip(IP ADDRESS HERE;//comment this out at home, but i need the school's ip address
// initialize the library instance:
EthernetClient client;

// if you don't want to use DNS (and reduce your sketch size)
// use the numeric IP instead of the name for the server:
IPAddress server(putthishere);    // numeric IP for api.cosm.com
//char server[] = "api.cosm.com";   // name address for cosm API

unsigned long lastConnectionTime = 0;          // last time you connected to the server, in milliseconds
boolean lastConnected = false;                 // state of the connection last time through the main loop
const unsigned long postingInterval = 10*1000; //delay between updates to Cosm.com

int doorUsedToBe;

boolean updatePie = false; 
boolean sent = false;



void setup() {
  // start serial port:
  Serial.begin(9600);
  
  pinMode(2, INPUT);
  digitalWrite(2, HIGH);
doorUsedToBe = digitalRead(2);

 // start the Ethernet connection:
  if (Ethernet.begin(mac) == 0) {
    Serial.println("Failed to configure Ethernet using DHCP");
    // DHCP failed, so use a fixed IP address:
    Ethernet.begin(mac, ip);
  }
 
 sendData(doorUsedToBe);
}

void loop() {
  int newDoorStatus = digitalRead(2);

if (newDoorStatus != doorUsedToBe)
{
  if (newDoorStatus == LOW && sent == false) {
    updatePie = true; 
    Serial.print("update pie : ");
    Serial.println(updatePie);
  }
  else if (newDoorStatus == HIGH && sent == true) {
    sent = false;
  }
  else {
  }
}
doorUsedToBe = newDoorStatus;
//Serial.println(updatePie);

  // if there's incoming data from the net connection.
  // send it out the serial port.  This is for debugging
  // purposes only:
  if (client.available()) {
    char c = client.read();
    Serial.print(c);
  }

//  // if there's no net connection, but there was one last time
//  // through the loop, then stop the client:
//  if (!client.connected() && lastConnected) {
//    Serial.println();
//    Serial.println("disconnecting.");
//    client.stop();
//  }

  // if you're not connected, and ten seconds have passed since
  // your last connection, then connect again and send data:
  if(!client.connected() && updatePie == true && ((millis() - lastConnectionTime) > postingInterval)) {
    sendData(newDoorStatus);
    Serial.println(newDoorStatus);
  }
  // store the state of the connection for next time through
  // the loop:
  lastConnected = client.connected();
}

// this method makes a HTTP connection to the server:
void sendData(int thisData) {
  // if there's a successful connection:
  if (client.connect(server, 80)) {
    Serial.println("connecting...");
    // send the HTTP PUT request:
    client.print("PUT /v2/feeds/");
    client.print(FEEDID);
    client.println(".csv HTTP/1.1");
    client.println("Host: api.cosm.com");
    client.print("X-ApiKey: ");
    client.println(APIKEY);
    client.print("User-Agent: ");
    client.println(USERAGENT);
    client.print("Content-Length: ");

    // calculate the length of the sensor reading in bytes:
    // 8 bytes for "sensor1," + number of digits of the data:
    int thisLength = 8 + getLength(thisData);
    client.println(thisLength);

    // last pieces of the HTTP PUT request:
    client.println("Content-Type: text/csv");
    client.println("Connection: close");
    client.println();

    // here's the actual content of the PUT request:
    client.print("sensor1,");
    client.println(thisData);
    updatePie = false;
    sent = true;

  } 
  else {
    // if you couldn't make a connection:
    Serial.println("connection failed");
    Serial.println();
    Serial.println("disconnecting.");
    client.stop();
  }
   // note the time that the connection was made or attempted:
  lastConnectionTime = millis();
}


// This method calculates the number of digits in the
// sensor reading.  Since each digit of the ASCII decimal
// representation is a byte, the number of digits equals
// the number of bytes:

int getLength(int someValue) {
  // there's at least one byte:
  int digits = 1;
  // continually divide the value by ten, 
  // adding one to the digit count for each
  // time you divide, until you're at 0:
  int dividend = someValue /10;
  while (dividend > 0) {
    dividend = dividend /10;
    digits++;
  }
  // return the number of digits:
  return digits;
}
