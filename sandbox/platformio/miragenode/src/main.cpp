#include "Ethernet.h"
#include "sACN.h"

uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//{0x90, 0xA2, 0xDA, 0x10, 0x14, 0x48}; // MAC Adress of your device
IPAddress ip(192, 168, 87, 62); // IP address of your device
IPAddress dns(8, 8, 8, 8); // DNS address of your device
IPAddress gateway(192,168,87,1); // Gateway address of your device
IPAddress subnet(255, 255, 255, 0); // Subnet mask of your device

EthernetUDP sacn;
Receiver recv(sacn, 1); // universe 1

// CID fd32aedc-7b94-11e7-bb31-be2e44b06b34
// uint8_t id[16] {0xFD, 0x32, 0xAE, 0xDC, 0x7B, 0x94, 0x11, 0xE7, 0xBB, 0x31, 0xBE, 0x2E, 0x44, 0xB0, 0x6B, 0x34};
//Source sender(sacn, 1, 100, id, "Arduino", true);

void dmxReceived() {
  Serial.println("New DMX data received ");
  Serial.print("DMX Slot 1: ");
  Serial.print(recv.dmx(1));
  Serial.print(" DMX Slot 2: ");
  Serial.println(recv.dmx(2));
}

void newSource() {
  Serial.print("new soure name: ");
	Serial.println(recv.name());
}

void framerate() {
  Serial.print("Framerate fps: ");
  Serial.println(recv.framerate());
}

void timeOut() {
	Serial.println("Timeout!");
}

void hardreset(uint8_t pinRST) {
  pinMode(pinRST, OUTPUT);
  digitalWrite(pinRST, HIGH);
  digitalWrite(pinRST, LOW);
  delay(1);
  digitalWrite(pinRST, HIGH);
  delay(150);
}

void setup() {
  
  Serial.begin(115200);
  Serial.println("Serial start");

	Ethernet.begin(mac, ip, dns, gateway, subnet);
  delay(1000);
  // Check for Ethernet hardware present
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }


	recv.callbackDMX(dmxReceived);
	recv.callbackSource(newSource);
	recv.callbackFramerate(framerate);
	recv.callbackTimeout(timeOut);
	recv.begin();
	// sender.begin();
	Serial.println("sACN start");

	// sender.dmx(1, 255); // send value 255 to DMX slot 1 
}

void loop() {
  recv.receive();
	// sender.idle();
}


