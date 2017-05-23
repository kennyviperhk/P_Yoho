/*
  SLIP-OSC.ino

  listen on USB Serial for slip encoded OSC packet
  to switch an LED on and off

  Depends on [PacketSerial](https://github.com/bakercp/PacketSerial)
  and [OSC](https://github.com/CNMAT/OSC/) libraries.

  Copyright Antoine Villeret - 2015

*/
#include <OSCBundle.h>
#include <PacketSerial.h>

//PacketSerial_<SLIP, SLIP::END, 8192> serial;
PacketSerial_<SLIP, SLIP::END, 128> serial;
//PacketSerial serial;

#define LEDPIN 13



#ifdef BOARD_HAS_USB_SERIAL
#include <SLIPEncodedUSBSerial.h>
SLIPEncodedUSBSerial SLIPSerial( thisBoardsSerialUSB );
#else
#include <SLIPEncodedSerial.h>
 SLIPEncodedSerial SLIPSerial(Serial);
#endif


void LEDcontrol(OSCMessage &msg)
{
  serial.setPacketHandler(&onPacket);
  serial.begin(115200);

  if (msg.isInt(0))
  {
    digitalWrite(LEDPIN, msg.getInt(0));

    
    OSCBundle bundleOUT;
    String outputAddress = "/a";
    String outputMsg = "load-|";
    bundleOUT.add("/a").add(" kjsadkasjdkas").add(outputMsg);


    //send the outgoing message
    SLIPSerial.beginPacket();
    bundleOUT.send(SLIPSerial);
    SLIPSerial.endPacket();
    bundleOUT.empty();
  }
  else {

    OSCBundle bundleOUT;
    String outputAddress = "/a";
    String outputMsg = "load-|";
    bundleOUT.add("/a").add(outputMsg);


    //send the outgoing message
    SLIPSerial.beginPacket();
    bundleOUT.send(SLIPSerial);
    SLIPSerial.endPacket();
    bundleOUT.empty();
  }
}

void setup() {
  // We must specify a packet handler method so that
  serial.setPacketHandler(&onPacket);
  serial.begin(115200);

  pinMode(LEDPIN, OUTPUT);
  digitalWrite(13, 1);
  delay(500);
  digitalWrite(13, 0);
}

void loop() {
  serial.update();
}

void onPacket(const uint8_t* buffer, size_t size) {
  OSCBundle bundleIN;

  for (int i = 0; i < size; i++) {
    bundleIN.fill(buffer[i]);
  }

  if (!bundleIN.hasError())
    bundleIN.dispatch("/led", LEDcontrol);
}

