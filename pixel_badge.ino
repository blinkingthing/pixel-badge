//#include <Tiny4kOLED.h>
#include <TinyWireM.h>

#define SHITTY_PIXEL   0x42              // 7 bit I2C address for ATTINY that controls LEDS
#define SHITTY_MEMORY   0x50              // 7 bit I2C address for CAAT24C03 EEPROM

byte mode = 3;
const int buttonPin = A2;
const int ledPin = 3;

int buttonState = 0;

int oldFunction;
int currentFunction = 3;
int oldValue;
int currentValue = 3;
int modeValue = 3;
int speedValue = 3;
int redValue = 3;
int greenValue = 3;
int blueValue = 3;
int limit;

uint8_t saodata[] = {0xFF, 0xFF};
uint8_t pixeldata[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
uint8_t modes[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x11, 0x12, 0x13, 0x14};
uint8_t speeds[] = {0x00, 0x01, 0xAA, 0xFF};
uint8_t red[] = {0x00, 0x01, 0xAA, 0xFF};
uint8_t green[] = {0x00, 0x01, 0xAA, 0xFF};
uint8_t blue[] = {0x00, 0x01, 0xAA, 0xFF};
uint8_t functions[] = {0x00, 0x01, 0x02, 0x03, 0x04};

byte dcyear;
byte maker;
byte saoid;

void reload() {
  TinyWireM.beginTransmission(SHITTY_PIXEL);
  TinyWireM.send(0x05);
  TinyWireM.send(0x52); //reload saved state
  TinyWireM.endTransmission();
  delay(200);
}

void save() {
  TinyWireM.beginTransmission(SHITTY_PIXEL);
  TinyWireM.send(0x05);
  TinyWireM.send(0x57); //reload saved state
  TinyWireM.endTransmission();
  delay(200);
}

void secret() {
  TinyWireM.beginTransmission(SHITTY_PIXEL);
  TinyWireM.send(0x01); //speed
  TinyWireM.send(0x01); //default
  TinyWireM.endTransmission();
  delay(200);
  TinyWireM.beginTransmission(SHITTY_PIXEL);
  TinyWireM.send(0x02); //red
  TinyWireM.send(0xFF); //full
  TinyWireM.endTransmission();
  delay(200);
  TinyWireM.beginTransmission(SHITTY_PIXEL);
  TinyWireM.send(0x03); //green
  TinyWireM.send(0xFF); //full
  TinyWireM.endTransmission();
  delay(200);
  TinyWireM.beginTransmission(SHITTY_PIXEL);
  TinyWireM.send(0x04); //blue
  TinyWireM.send(0xFF); //full
  TinyWireM.endTransmission();
  delay(200);
  TinyWireM.beginTransmission(SHITTY_PIXEL);
  TinyWireM.send(0x00); //mode
  TinyWireM.send(0x42); //flag
  TinyWireM.endTransmission();
  delay(200);
}

void indicateFunction() {
  for (int i = 1; i < (currentFunction + 1); i++) {
    digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(50);                       // wait for a second
    digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
    delay(50);
  }
}

void indicateValue() {
  for (int i = 0; i < (currentValue + 1); i++) {
    digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(50);                       // wait for a second
    digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
    delay(50);
  }
}

void indicateButtonState() {
  int blinks = round(buttonState / 100);
  for (int i = 1; i < blinks; i++) {
    digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);                       // wait for a second
    digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
    delay(1000);
  }
}

void readSAO() {
  int count = 0;
  TinyWireM.beginTransmission(SHITTY_MEMORY);
  TinyWireM.send(0x00);
  TinyWireM.endTransmission();
  TinyWireM.requestFrom(0x50, 2);
  while (TinyWireM.available() && count < 2) {
    saodata[count] = TinyWireM.receive();
    count++;
  }
}

void readPixel() {
  int count = 0;
  TinyWireM.beginTransmission(SHITTY_PIXEL);
  TinyWireM.send(0x00);
  TinyWireM.endTransmission();
  TinyWireM.requestFrom(SHITTY_PIXEL, 6);
  while (TinyWireM.available() && count < 6) {
    pixeldata[count] = TinyWireM.receive();
    count++;
  }
}

void updatePixel() {
  TinyWireM.beginTransmission(SHITTY_PIXEL);
  TinyWireM.send(currentFunction); //function


  // do something different depending on the range value:
  switch (currentFunction) {
    case 0:    // modes
      TinyWireM.send(modes[modeValue]); //function
      break;
    case 1:    // speeds
      TinyWireM.send(speeds[speedValue]); //function
      break;
    case 2:    // reds
      TinyWireM.send(red[redValue]); //function
      break;
    case 3:    // greens
      TinyWireM.send(green[greenValue]); //function
      break;
    case 4:    // blues
      TinyWireM.send(blue[blueValue]); //function
      break;
  }

  TinyWireM.endTransmission();
  delay(200);
}

void increaseValue() {
  // do something different depending on the range value:
  switch (currentFunction) {
    case 0:    // modes
      currentValue = modeValue; //function
      break;
    case 1:    // speeds
      currentValue = speedValue; //function
      break;
    case 2:    // reds
      currentValue = redValue; //function
      break;
    case 3:    // greens
      currentValue = greenValue; //function
      break;
    case 4:    // blues
      currentValue = blueValue; //function
      break;
  }
  if (currentFunction == 0) {
    limit = 13;
  } else {
    limit = 3;
  }
  if (currentValue <= limit) {
    currentValue++;
  } else {
    currentValue = 0;
  }

  switch (currentFunction) {
    case 0:    // modes
      modeValue = currentValue; //function
      break;
    case 1:    // speeds
      speedValue = currentValue; //function
      break;
    case 2:    // reds
      redValue = currentValue; //function
      break;
    case 3:    // greens
      greenValue = currentValue; //function
      break;
    case 4:    // blues
      blueValue = currentValue; //function
      break;
  }
}

void decreaseValue() {
  // do something different depending on the range value:
  switch (currentFunction) {
    case 0:    // modes
      currentValue = modeValue; //function
      break;
    case 1:    // speeds
      currentValue = speedValue; //function
      break;
    case 2:    // reds
      currentValue = redValue; //function
      break;
    case 3:    // greens
      currentValue = greenValue; //function
      break;
    case 4:    // blues
      currentValue = blueValue; //function
      break;
  }

  if (currentFunction == 0) {
    limit = 13;
  } else {
    limit = 3;
  }
  if (currentValue >= 1) {
    currentValue--;
  } else {
    currentValue = limit;
  }

  switch (currentFunction) {
    case 0:    // modes
      modeValue = currentValue; //function
      break;
    case 1:    // speeds
      speedValue = currentValue; //function
      break;
    case 2:    // reds
      redValue = currentValue; //function
      break;
    case 3:    // greens
      greenValue = currentValue; //function
      break;
    case 4:    // blues
      blueValue = currentValue; //function
      break;
  }
}

void increaseFunction() {

  if (currentFunction <= 3) {
    currentFunction++;
  } else {
    currentFunction = 0;
  }
}

void decreaseFunction() {
  if (currentFunction >= 1) {
    currentFunction--;
  } else {
    currentFunction = 4;
  }
}


void setup() {
  // put your setup code here, to run once:
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  pinMode(ledPin, OUTPUT);

  TinyWireM.begin();

  for (int i = 1; i < 5; i++) {
    digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(50);                       // wait for a second
    digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
    delay(50);
  }

  //delay(100);
  //readSAO();
  //delay(100);
  //readPixel();

}

void loop() {
  // put your main code here, to run repeatedly:

  // read the state of the pushbutton value:
  buttonState = analogRead(buttonPin);
  oldFunction = currentFunction;
  oldValue = currentValue;



  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState > 300) {
   // indicateButtonState();
    if (buttonState > 1000) {
      
      // button func - 
      increaseFunction();
      //decreaseFunction();
    } else if ((buttonState < 1000) && (buttonState > 760)) {
      // button ?:
      secret();
      
    } else if ((buttonState < 760) && (buttonState > 650)) {
      // button - :
      decreaseValue();
      digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)

    } else if ((buttonState < 650) && (buttonState > 570)) {
      // button + :
      increaseValue();
      digitalWrite(ledPin, HIGH);   // turn the LED on (HIGH is the voltage level)
    } else if ((buttonState < 570) && (buttonState > 520)) {
      // button save:
      save();
      
    } else {
      //nothing
    }

    //update the pixel
    updatePixel();

    //update info from pixel eeprom
    //readPixel();

  } else {
    digitalWrite(ledPin, LOW);    // turn the LED off by making the voltage LOW
  }




  if (currentFunction != oldFunction) {
    indicateFunction();
  }
  if (currentValue != oldValue) {
    indicateValue();
  }
  delay(10);

}
