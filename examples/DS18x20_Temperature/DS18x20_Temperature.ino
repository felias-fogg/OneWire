// -*-c++-*-
// OneWire DS18S20, DS18B20, DS1822 Temperature Example
//
// http://www.pjrc.com/teensy/td_libs_OneWire.html
//
// The DallasTemperature library can do all this work for you!
// http://milesburton.com/Dallas_Temperature_Control_Library
//
// works also on an ATtiny using the SoftwareSerial library.
//
// If you just want to see that the devices are recognized,
// simply connect a LED:
// Blinks once, when there are no more devices, 
// twice for each DS18S20
// three times for each DS18B20
// four times for each DS1822
// five times for any other device

#include <OneWire.h>

#define DS1820DATA 2
#define DS1820VCC 3
#define LED 4
#define SERIALRX 7 // only needed on ATtiny
#define SERIALTX 6 // only needed on ATtiny
#define SPEED 9600 // since we want it also running with 1MHz

#if __AVR_ARCH__==2 ||  __AVR_ARCH__==25
#include <SoftwareSerial.h>
SoftwareSerial Serial(SERIALRX, SERIALTX);
#endif
OneWire ds(DS1820DATA);

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  pinMode(LED, OUTPUT);
  pinMode(DS1820VCC, OUTPUT);
  digitalWrite(DS1820VCC, HIGH);
  Serial.begin(SPEED);
}

void loop(void) {
  byte i;
  byte present = 0;
  byte type_s;
  byte data[12];
  byte addr[8];
  float celsius, fahrenheit;

  if ( !ds.search(addr)) {
    Serial.println("No more addresses.");
    Serial.println();
    ds.reset_search();
    blink(1);
    delay(1000);
    return;
  }
  
  Serial.print("ROM =");
  for( i = 0; i < 8; i++) {
    Serial.write(' ');
    Serial.print(addr[i], HEX);
  }

  if (OneWire::crc8(addr, 7) != addr[7]) {
      Serial.println("CRC is not valid!");
      return;
  }
  Serial.println();
 
  // the first ROM byte indicates which chip
  switch (addr[0]) {
    case 0x10:
      Serial.println("  Chip = DS18S20");  // or old DS1820
      type_s = 1;
      blink(2);
      break;
    case 0x28:
      Serial.println("  Chip = DS18B20");
      type_s = 0;
      blink(3);
      break;
    case 0x22:
      Serial.println("  Chip = DS1822");
      type_s = 0;
      blink(4);
      break;
    default:
      Serial.println("Device is not a DS18x20 family device.");
      blink(5);
      return;
  } 

  ds.reset();
  ds.select(addr);
  ds.write(0x44,1);         // start conversion, with parasite power on at the end
  
  delay(1000);     // maybe 750ms is enough, maybe not
  // we might do a ds.depower() here, but the reset will take care of it.
  
  present = ds.reset();
  ds.select(addr);    
  ds.write(0xBE);         // Read Scratchpad

  Serial.print("  Data = ");
  Serial.print(present,HEX);
  Serial.print(" ");
  for ( i = 0; i < 9; i++) {           // we need 9 bytes
    data[i] = ds.read();
    Serial.print(data[i], HEX);
    Serial.print(" ");
  }
  Serial.print(" CRC=");
  Serial.print(OneWire::crc8(data, 8), HEX);
  Serial.println();

  // convert the data to actual temperature

  unsigned int raw = (data[1] << 8) | data[0];
  if (type_s) {
    raw = raw << 3; // 9 bit resolution default
    if (data[7] == 0x10) {
      // count remain gives full 12 bit resolution
      raw = (raw & 0xFFF0) + 12 - data[6];
    }
  } else {
    byte cfg = (data[4] & 0x60);
    if (cfg == 0x00) raw = raw << 3;  // 9 bit resolution, 93.75 ms
    else if (cfg == 0x20) raw = raw << 2; // 10 bit res, 187.5 ms
    else if (cfg == 0x40) raw = raw << 1; // 11 bit res, 375 ms
    // default is 12 bit resolution, 750 ms conversion time
  }
  celsius = (float)raw / 16.0;
  fahrenheit = celsius * 1.8 + 32.0;
  Serial.print("  Temperature = ");
  Serial.print(celsius);
  Serial.print(" Celsius, ");
  Serial.print(fahrenheit);
  Serial.println(" Fahrenheit");
}

void blink(int num) {
  for (int i=0; i < num; i++) {
    digitalWrite(LED, HIGH);
    delay(250);
    digitalWrite(LED, LOW);
    delay(250);
  }
  delay(250);
}
