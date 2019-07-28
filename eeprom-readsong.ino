
#include <Wire.h>
#define buzzerPin PB4

#define ext_eeprom 0x50    // address of eeprom chip

int tempo = 120;

void setup() {

  Serial.begin(9600);
  Wire.begin();

  unsigned int address = 0;

  Serial.println("Reading song from eeprom, connect now...");
  Serial.println("");
  delay(1000);

  readonce();
}

byte readEEPROM(int deviceaddress, unsigned int eeaddress )
{
  byte rdata;

  Wire.beginTransmission(deviceaddress);
  Wire.write((int)(eeaddress >> 8));   // MSB
  Wire.write((int)(eeaddress & 0xFF)); // LSB
  Wire.endTransmission();
  Wire.requestFrom(deviceaddress, 1);
  if (Wire.available()) {
    rdata = Wire.read();
  }
  delay(10);
  return rdata;
}

void readonce() {

  int eepromaddress, mynote, myduration;
  for (int thisone = 0; thisone < 511; thisone++) {
    int mynote, myduration;
    byte readhigh, readlow;

    eepromaddress = thisone * 2;
    readhigh = readEEPROM(ext_eeprom, eepromaddress);
    readlow = readEEPROM(ext_eeprom, eepromaddress + 1);
    mynote = word(readhigh,readlow);
    thisone++; //duration

    eepromaddress = thisone * 2;
    readhigh = readEEPROM(ext_eeprom, eepromaddress);
    readlow = readEEPROM(ext_eeprom, eepromaddress + 1);
 
    myduration = word(readhigh,readlow)*tempo;

    if (mynote == 17)          //  rest 
    {
      delay(myduration);            // then pause for a moment
    }
    else                          // otherwise, play the note
    {
      tone(buzzerPin, mynote, myduration);
      delay(myduration);            // wait for tone to finish
    }
    delay(tempo/10);              // brief pause between notes
  }
}

void loop() {
}
