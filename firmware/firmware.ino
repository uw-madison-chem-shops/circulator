// firmware: circulator-driver

// 7SEG display (I2C communication)
#include <Wire.h> // Include the Arduino SPI library
const byte s7sAddress = 0x71;
char tempString[10];  // used with sprintf to create strings

// rotary encoder
#include <Encoder.h>
Encoder knob(1, 0);  // interrupt, registered in library
long frequency  = 2000;  // mHz

// control pins
int relay1 = 2;  // physical pin 4
int relay2 = A3;  // physical pin 26
int relay3 = A2;  // physical pin 25
int relay4 = A1;  // physical pin 24
int relay5 = A0;  // physical pin 23

// buttons
int bypass_button = 7;  // physical pin 13
int bypass_LED = 9;  // physical pin 15
int run_button = 6;  // physical pin 12
int run_LED = 8;  // physical pin 14

// TTL
int TTL1 = 4;  // physical pin 6
int TTL1_state = HIGH;
int TTL2 = 3;  // physical pin 5
int TTL2_state = HIGH;

// run control
bool bypass_state = LOW;
bool run_state = LOW;

// timing control
bool state = LOW;
float period = 1000;           // interval at which to blink (milliseconds)
unsigned long previousMillis = 0;        // will store last time LED was updated

void setup()
{
  // 7SEG display (I2C communication)
  Wire.begin();  // Initialize hardware I2C pins
  clearDisplayI2C();  // Clears display, resets cursor
  s7sSendStringI2C("-HI-");
  setDecimalsI2C(0b111111);  // Turn on all decimals, colon, apos
  setBrightnessI2C(255);  // 0 to 255
  clearDisplayI2C();
  setDecimalsI2C(0b00000010);
  sprintf(tempString, "%04d", frequency / 10);
  s7sSendStringI2C(tempString);
  // rotary encoder
  knob.write(0);
  // serial
  Serial.begin(9600);
  // output pins
  pinMode(relay1, OUTPUT);
  pinMode(relay2, OUTPUT);
  pinMode(relay3, OUTPUT);
  pinMode(relay4, OUTPUT);
  pinMode(relay5, OUTPUT);
  pinMode(bypass_LED, OUTPUT);
  pinMode(run_LED, OUTPUT);
  // input pins
  pinMode(bypass_button, INPUT_PULLUP);
  pinMode(run_button, INPUT_PULLUP);
  pinMode(TTL1, INPUT_PULLUP);
  pinMode(TTL2, INPUT_PULLUP);
  //
  delay(1000);
}

void loop() {
  // buttons (active low)
  if (digitalRead(bypass_button) == LOW) {
    while(digitalRead(bypass_button) == LOW) {
      delay(1);  // ms
    }
    bypass_state = !bypass_state;
  }
  digitalWrite(bypass_LED, !bypass_state);  // active low
  if (digitalRead(run_button) == LOW) {
    while(digitalRead(run_button) == LOW) {
      delay(1);  // ms
    }
    run_state = !run_state;
  }
  // TTL
  int ttl1 = digitalRead(TTL1);
  if (ttl1 != TTL1_state) {
    TTL1_state = ttl1;
    run_state = !ttl1;
  }
  int ttl2 = digitalRead(TTL2);
  if (ttl2 != TTL2_state) {
    TTL2_state = ttl2;
    bypass_state = !ttl2;
  }
  digitalWrite(run_LED, !run_state);  // active low
  // input from rotary encoder
  int x = knob.read();
  int sign = (x > 0) - (x < 0);
  if (abs(x) >= 4) {
    if (frequency == 25000) {
      if (sign == -1) {
        frequency -= 1000;
      }
    }
    else if (10000 < frequency) {
      frequency += 1000 * sign;
    }
    else if (frequency == 10000) {
      if (sign == 1) frequency += 1000;
      if (sign == -1) frequency -= 100;
    }
    else if ((1000 < frequency) && (frequency < 10000)) {
      frequency += 100 * sign;
    }
    else if (frequency == 1000) {
      if (sign == 1) frequency += 100;
      if (sign == -1) frequency -= 10;
    }
    else if ((20 <= frequency) && (frequency < 1000)) {
      frequency += 10 * sign;
    }
    else{
      if (sign == 1) {
        frequency += 10;
      }
    }
    sprintf(tempString, "%04d", frequency / 10);
    s7sSendStringI2C(tempString);
    knob.write(0);
    period = 1e6 / (float)frequency;
  }
  // control cycle relays
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= (period / 2)) {
    previousMillis = currentMillis;
    if (run_state == HIGH) {
      state = !state;
    }
    digitalWrite(relay1, state);  // A
    digitalWrite(relay4, state);  // 2, 4
    digitalWrite(relay2, !state);  // B
    digitalWrite(relay3, !state);  // 1, 3
  }
  // control bypass relay
  digitalWrite(relay5, !bypass_state);  // low active
}

void s7sSendStringI2C(String toSend) {
  // This custom function works somewhat like a serial.print.
  //  You can send it an array of chars (string) and it'll print
  //  the first 4 characters in the array.
  Wire.beginTransmission(s7sAddress);
  for (int i=0; i<4; i++) {
    Wire.write(toSend[i]);
  }
  Wire.endTransmission();
}

void clearDisplayI2C() {
  // Send the clear display command (0x76)
  //  This will clear the display and reset the cursor
  Wire.beginTransmission(s7sAddress);
  Wire.write(0x76);  // Clear display command
  Wire.endTransmission();
}

void setBrightnessI2C(byte value) {
  // Set the displays brightness. Should receive byte with the value
  //  to set the brightness to
  //  dimmest------------->brightest
  //     0--------127--------255
  Wire.beginTransmission(s7sAddress);
  Wire.write(0x7A);  // Set brightness command byte
  Wire.write(value);  // brightness data byte
  Wire.endTransmission();
}

void setDecimalsI2C(byte decimals) {
  // Turn on any, none, or all of the decimals.
  //  The six lowest bits in the decimals parameter sets a decimal 
  //  (or colon, or apostrophe) on or off. A 1 indicates on, 0 off.
  //  [MSB] (X)(X)(Apos)(Colon)(Digit 4)(Digit 3)(Digit2)(Digit1)
  Wire.beginTransmission(s7sAddress);
  Wire.write(0x77);
  Wire.write(decimals);
  Wire.endTransmission();
}
