#include <Wire.h>
#include "ST7036.h"
#include "bin.h"
#include "PinChangeInt.h"

//Hardware required:
//1: I2C LCD connected on standard pins
//2: Linear Pot connected on analog pin 1
//3: Jumbo Servo on I2C
//4: Push button connected to GND and pin 9

long sensorValue = 0;  // variable to store the value coming from the sensor
long angle = 0;
ST7036 lcd = ST7036 ( 2, 16, 0x7C );                               //Gotcha with the address, needed to be shifted one to the left

const int slaveAddress = 8;
const int triggerPin = 9;
const int switchPin = 11;
const int potPin = A0;
const int speedValue = 130; // 0 to 255

volatile byte triggered = 0;

enum registers {ReadReg = 0,
                Status = 0,
                Position = 1,
                Angle = 2,
                Speed = 3}; 

const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int readIndex = 0;              // the index of the current reading
int total = 0;                  // the running total
int average = 0;                // the average

//Large chars
char bn1[]={6,1,6, 1,6,32, 1,3,4, 3,3,4, 6,2,2,   6,3,1, 5,3,1, 1,1,4,   5,3,4, 5,3,4};
char bn2[]={6,2,6, 2,6,2,  5,2,2, 2,2,4, 32,6,32, 2,2,4, 5,2,4, 32,6,32, 5,3,4, 32,32,6};

void setup()
{
    pinMode(triggerPin, INPUT_PULLUP);  // Configure the pin as an input, and turn on the pullup resistor.
                                      // See http://arduino.cc/en/Tutorial/DigitalPins
    attachPinChangeInterrupt(triggerPin, interruptFunction, FALLING);

    lcd.init();
    lcd.on();
    lcd.setCursor(0, 0);
   
    lcd.print("Hello BCS!");
    
    loadlargechars();
    
    delay(2000);
    lcd.clear();
}

// Do not use any Serial.print() in interrupt subroutines. Serial.print() uses interrupts,
// and by default interrupts are off in interrupt subroutines. Interrupt routines should also
// be as fast as possible. Here we just increment a counter.
void interruptFunction() {
  triggered = 1;
}

void loadlargechars()
{
  uint8_t cc1[8] = { // Custom Character 1
     B8(11111),
     B8(11111),
     B8(11111),
     B8(00000),
     B8(00000),
     B8(00000),
     B8(00000),
     B8(00000)
     };

  uint8_t cc2[8] = { // Custom Character 2
     B8(00000),
     B8(00000),
     B8(00000),
     B8(00000),
     B8(00000),
     B8(11111),
     B8(11111),
     B8(11111)
     };

  uint8_t cc3[8] = { // Custom Character 3
     B8(11111),
     B8(11111),
     B8(11111),
     B8(00000),
     B8(00000),
     B8(00000),
     B8(11111),
     B8(11111)
     };

  uint8_t cc4[8] = { // Custom Character 4
     B8(11100),
     B8(11110),
     B8(11111),
     B8(11111),
     B8(11111),
     B8(11111),
     B8(11110),
     B8(11100)
     };
     
  uint8_t cc5[8] = { // Custom Character 5
     B8(00111),
     B8(01111),
     B8(11111),
     B8(11111),
     B8(11111),
     B8(11111),
     B8(01111),
     B8(00111)
     };     

  uint8_t cc6[8] = { // Custom Character 6
     B8(11111),
     B8(11111),
     B8(11111),
     B8(11111),
     B8(11111),
     B8(11111),
     B8(11111),
     B8(11111)
     };   

  uint8_t cc7[8] = { // Custom Character 7 Degree
     B8(01110),
     B8(10001),
     B8(10001),
     B8(10001),
     B8(01110),
     B8(00000),
     B8(00000),
     B8(00000)
     };  
    
    lcd.load_custom_character(1, cc1);
    lcd.load_custom_character(2, cc2);
    lcd.load_custom_character(3, cc3);
    lcd.load_custom_character(4, cc4);
    lcd.load_custom_character(5, cc5);
    lcd.load_custom_character(6, cc6);
    lcd.load_custom_character(7, cc7);
}

int getReading()
{
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = analogRead(potPin);
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  average = total / numReadings;
  return average;
}

void loop()
{
  if (triggered == 1) {
    lcd.clear();
    lcd.print("Sending");
    writeRegister(Speed,speedValue);
    writeRegister(Angle,sensorValue); //Values 0-1023
    delay(700);
    lcd.clear();
    lcd.print("Sent");
    delay(500);
    lcd.clear();
    display(angle);
    triggered = 0;    
  }

  
  // read the value from the sensor:
  long newValue = getReading();
  if (sensorValue != newValue) {
    // Makes the assumption that the pot turns 360 degrees, adjust the value below to compensate e.g. a 300 degree pot would be 3000
    angle = (sensorValue * 3600 / 1024); 
    display(angle);
  }
  sensorValue = newValue;

  delay(10);
}

void display(int i) //Display the value in large characters
{
  lcd.clear();  
  //lcd.print(angle);
  PrintBigDeg(15);
  if (i == 0)
  {
    PrintBigDigit(0,8);
    PrintBigDot(11);
    PrintBigDigit(0,12);
  }
  int pos = 15;
  while(i>0)
  {
    pos -= 3;
    PrintBigDigit(i % 10,pos);
    i /= 10;
    if (pos==12){
     PrintBigDot(--pos);
    }
  }
  

}

void PrintBigDigit(uint8_t digit, uint8_t offset)
{

 // Line 1 of the one digit number
 lcd.setCursor(0,offset);
 lcd.write(bn1[digit*3]);
 lcd.write(bn1[digit*3+1]);
 lcd.write(bn1[digit*3+2]);

 // Line 2 of the one-digit number
 lcd.setCursor(1,offset);
 lcd.write(bn2[digit*3]);
 lcd.write(bn2[digit*3+1]);
 lcd.write(bn2[digit*3+2]);
}

void PrintBigDot(uint8_t offset)
{
  lcd.setCursor(1,offset);
  lcd.print('.');
}
void PrintBigDeg(int8_t offset)
{
  lcd.setCursor(0,offset);
  lcd.write(7);
}

void readRegister(byte reg) {
  Serial.print("Reading register:");
  Serial.print(reg);
  
  Wire.beginTransmission(slaveAddress);
  Wire.write((uint8_t *)&reg, sizeof(reg));
  Wire.endTransmission();
  delay(100);
  Wire.requestFrom(slaveAddress, 2);    // request 2 bytes from slave device #8

  int value;
  if (Wire.available()) { // slave may send less than requested
    value = Wire.read(); // receive a byte as character
  }
  if (Wire.available()) {
    int high = Wire.read();
    value = value + (high << 8);
  }

  Serial.print("=");    
  Serial.println(value);    

  while (Wire.available()) { // clear buffer incase anything else sent
    char c = Wire.read(); // receive a byte as character
  }
  
  delay(500);
}

void writeRegister(byte command,int value) {
  Serial.print("Writing register:");
  Serial.print(command);
  Serial.print("=");    
  Serial.println(value);    
  char message[3];
  message[0] = command;
  message[1] = value;       //low Byte, high
  message[2] = value >> 8;
  Wire.beginTransmission(slaveAddress);
  Wire.write(message,3);
  Wire.endTransmission();
}


