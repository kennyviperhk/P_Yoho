#include <EEPROM.h>

#define Offset_long 4



//Mapping
//Stepper Setup
#define HOME_MAXSPEED     int_array[0]
#define HOME_ACCELERATION     int_array[1]
//VU Setting
#define MAX_SPEED      int_array[2]
#define ACCELERATION       int_array[3]
#define MAX_POSITION      int_array[4]


void Load_Flash()
{
  //HOME
  HOME_MAXSPEED =  EEPROMReadInt(0);
  HOME_ACCELERATION =  EEPROMReadInt(1);
  //RUNNING
  MAX_SPEED =  EEPROMReadInt(2);
  ACCELERATION  =  EEPROMReadInt(3);
  MAX_POSITION =  EEPROMReadInt(4);

}
void Write_Flash()
{
  //HOME
  EEPROMWriteInt(0, HOME_MAXSPEED); //Homing Speed
  EEPROMWriteInt(1, HOME_ACCELERATION); //Not in Use
  //RUNNING
  EEPROMWriteInt(2, MAX_SPEED); //Base Speed of DEW
  EEPROMWriteInt(3, ACCELERATION ); //Noise Floor (8)
  EEPROMWriteInt(4, MAX_POSITION); //Gain of Voice (300)

}


// ============ WRITE/READ Method ================

byte low1Byte = 0;
byte low2Byte = 0;
byte low3Byte = 0;
byte low4Byte = 0;

void EEPROMWriteInt(unsigned int p_address, int p_value)
{
  unsigned int pc_address = p_address * 4 + Offset_long;
  low1Byte = ((p_value >> 0) & 0xFF);
  low2Byte = ((p_value >> 8) & 0xFF);
  low3Byte = ((p_value >> 16) & 0xFF);
  low4Byte = ((p_value >> 24) & 0xFF);
  // Serial.print("Write Flash: ");
  //Serial.print(pc_address, DEC);
  //Serial.print(": ");
  //Serial.println(p_value, DEC);
  EEPROM.write(pc_address, low1Byte);
  EEPROM.write(pc_address + 1, low2Byte);
  EEPROM.write(pc_address + 2, low3Byte);
  EEPROM.write(pc_address + 3, low4Byte);
}
//This function will read a 2 byte integer from the eeprom at the specified address and address + 1
int EEPROMReadInt(unsigned int p_address)
{
  unsigned int pc_address = p_address * 4 + Offset_long;
  low1Byte = EEPROM.read(pc_address);
  low2Byte = EEPROM.read(pc_address + 1);
  low3Byte = EEPROM.read(pc_address + 2);
  low4Byte = EEPROM.read(pc_address + 3);
  //Serial.println("Read Flash:");
  //Serial.println(low1Byte, HEX);
  //Serial.println(low2Byte, HEX);
  // Serial.println(low3Byte, HEX);
  //Serial.println(low4Byte, HEX);
  //  Serial.println(low3Byte, HEX);
  //  Serial.println(highByte, HEX);
  return ((low1Byte << 0) & 0x000000FF) + ((low2Byte << 8) & 0x0000FF00) + ((low3Byte << 16) & 0x00FF0000) + ((low4Byte << 24) & 0xFF000000);
}


