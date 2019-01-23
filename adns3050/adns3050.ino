#include <SPI.h>

// define registers
#define DELTA_X_REG 0x03
#define DELTA_y_REG 0x04
#define RESET_REG 0x3a
#define MOUSE_CTRL_REG 0x0d
#define MOTION_CTRL_REG 0x41
#define PROD_ID_REG 0x00

// read/write address
#define WRITE 0x80
#define READ 0x00

// set slave select to pin 10
//const int slaveSelectPin = 10;
#define slaveSelectPin 10

void setup() {
  Serial.begin(115200);
  //byte out = 0;
  //byte read = 0;
  //byte bit = 0;
  
  // set slaveSelectPin as output 
  pinMode(slaveSelectPin, OUTPUT);
  
  // initialise SPI with parameters  
  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE3);
  SPI.setClockDivider(SPI_CLOCK_DIV16);
  delay(10);

  
  // power up reset
  digitalWrite(slaveSelectPin, HIGH);
  delay(20);
  digitalWrite(slaveSelectPin, LOW);
  writeTo(RESET_REG, 0x5a);
  delay(100);
  writeTo(MOUSE_CTRL_REG, 0x20);
  //delay(10);
  writeTo(MOTION_CTRL_REG, 0x00);
  delay(100);

  //test connection
  int test = (int) readFrom(PROD_ID_REG,1);
  if (test == 9){
    Serial.println("connection succesful");
    }

}

void loop() {
  int x = convTwosComp(readFrom(DELTA_X_REG,1));
  int y = convTwosComp(readFrom(DELTA_y_REG,1)); 
  Serial.print("dx: ");
  Serial.print( x );
  Serial.print(" dy: ");
  Serial.println( y ); 
  delay(100);

}

// send command to chip
void writeTo(byte reg, byte value){
  // make address with reg and write bit
  byte address = reg | WRITE;
  // select device
  digitalWrite(slaveSelectPin, LOW);

  SPI.transfer(address); // send address
  SPI.transfer(value); // send value
  delayMicroseconds(30);
  // de-select device
  digitalWrite(slaveSelectPin, HIGH);
  delayMicroseconds(30);
}

// recieve data from chip
byte readFrom(byte reg, int byteNo){
  // make address with reg and read bit
  //byte address = reg | READ;
  byte address = reg & 0x7f;
  // select device
  digitalWrite(slaveSelectPin, LOW);

  SPI.transfer(address); // send address
  delayMicroseconds(100);
  // recieve first byte
  byte result = SPI.transfer(0x00);
  // recieve remaining bytes and form single number
  byteNo--;
  if (byteNo > 0){
    result = result << 8; // shift to the left one byte
    delayMicroseconds(30);
    byte inByte = SPI.transfer(0x00); // recieve the first byte
    result = result | inByte;
    byteNo--;     
  }
  // de-select
  delayMicroseconds(30);
  digitalWrite(slaveSelectPin, HIGH);
  delayMicroseconds(30);
  // return result
  return result;
}

int convTwosComp(byte b){
  int num = (int) b;
  if(b & 0x80){
    num = num - 256;
    }
  return num;
}

int convTwosComp2(int b){ //Convert from 2's complement
   if(b & 0x80){
     b = -1 * ((b ^ 0xff) + 1);
     }
   return b;
   }




