#include <Wire.h>
#include <FastLED.h>

int i2cdata[32];              //i2cdata[0] = index 0 is the register offset. useful for selection and commands
int i2cnumbytes=32;           //number of bytes in i2c

int detect1,detect2,detect3,detect4;
int timeout = 20000;  //timeout 20ms
int cm_previous = 0; 

int trigPin1 = 13;    // Trigger
int echoPin1 = 12;    // Echo
long duration1, cm1;

int trigPin2 = 11;    // Trigger
int echoPin2 = 10;    // Echo
long duration2, cm2;

int trigPin3 = A2;    // Trigger
int echoPin3 = A3;    // Echo
long duration3, cm3;

int trigPin4 = 9;    // Trigger
int echoPin4 = 8;    // Echo
long duration4, cm4;

int ledpwm = 3;
int motorpwm = 5;

#define NUM_LEDS 8
#define DATA_PIN 7
CRGB leds[NUM_LEDS];
CRGBPalette16 currentPalette;
TBlendType    currentBlending;
int brightness = 50;
int detectiondistance = 25;

int i=0;
int serial_enable = 0;
int led_enable = 0;
int ledrgb_enable = 0;
int ledrgbdetect_enable =1;
int motor_enable =0;

void setup() {
  Wire.begin(8);                // join i2c bus with address #8
  Wire.onRequest(requestEvent); // register event
  Wire.onReceive(receiveEvent); // register event

  //Serial Port begin
  Serial.begin(115200);
  Serial.println("Ultrasonic,LED & RGB I2C Slave Ready: Add 0x08");

  //Define inputs and outputs
  pinMode(trigPin1, OUTPUT);
  pinMode(echoPin1, INPUT);
  //Define inputs and outputs
  pinMode(trigPin2, OUTPUT);
  pinMode(echoPin2, INPUT);
  //Define inputs and outputs
  pinMode(trigPin3, OUTPUT);
  pinMode(echoPin3, INPUT);
  //Define inputs and outputs
  pinMode(trigPin4, OUTPUT);
  pinMode(echoPin4, INPUT);
  
  
  analogWrite(ledpwm,1);     //start with dimm light .
  //analogWrite(motorpwm,150);     //on motor pump 

  
  FastLED.addLeds<WS2812, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(brightness);
  currentPalette = RainbowColors_p;
  for(int i;i<1000;i++){
    FillLEDsFromPaletteColors(i,80);
    FastLED.show();
  }
  brightness = 15;
  ledrgb(CRGB::Blue,brightness,0,1);
  ledrgb(CRGB::Black,15,2,5);
  ledrgb(CRGB::Blue,brightness,6,7);

  analogWrite(motorpwm,0);     //off motor pump 

  
}
 
void loop() {

  
  detect1 = obstacle_sensor(trigPin1,echoPin1);
  delay(1);
  detect2 = obstacle_sensor(trigPin2,echoPin2);
  delay(1);
  detect3 = obstacle_sensor(trigPin3,echoPin3);
  delay(1);
  detect4 = obstacle_sensor(trigPin4,echoPin4);
  delay(1);

  if(ledrgbdetect_enable==1)
    {
  if(detect1<=detectiondistance){
    ledrgb(CRGB::Red,brightness,6,6);
    }
  else {
    ledrgb(CRGB::Blue,brightness,6,6);
    }
  if(detect2<=detectiondistance){
    ledrgb(CRGB::Red,brightness,1,1);
    }
  else {
    ledrgb(CRGB::Blue,brightness,1,1);
    }
  if(detect3<=detectiondistance){
    ledrgb(CRGB::Red,brightness,7,7);
    }
  else {
    ledrgb(CRGB::Blue,brightness,7,7);
    }
  if(detect4<=detectiondistance){
    ledrgb(CRGB::Red,brightness,0,0);
    }
  else {
    ledrgb(CRGB::Blue,brightness,0,0);
    }
   
}
  
if(serial_enable==1){
    Serial.print(detect1);
    Serial.print(" ");
    Serial.print(detect2);
    Serial.print(" ");
    Serial.print(detect3);
    Serial.print(" ");
    Serial.print(detect4);
    Serial.print(" ");
    Serial.println();
    //delay(50);
    serial_enable=0;
  }
if (led_enable == 1){
    analogWrite(ledpwm,i2cdata[1]);
  //  Serial.println("sss");
    led_enable = 0;
    
  }

if (motor_enable == 1){
    analogWrite(motorpwm,i2cdata[1]);
  //  Serial.println("sss");
    motor_enable = 0;
    
  }
if (ledrgb_enable == 1){
    if(i2cdata[1]==1){
      ledrgb(CRGB::White,brightness,2,5);
    }
    else if(i2cdata[1]==2){
      ledrgb(CRGB::Red,brightness,2,5);
    }
    else if(i2cdata[1]==3){
      ledrgb(CRGB::Green,brightness,2,5);
    }
    else if(i2cdata[1]==4){
      ledrgb(CRGB::Cyan,brightness,2,5);
    }
    else if(i2cdata[1]==0){
      ledrgb(CRGB::Black,brightness,2,5);
    }
    else if(i2cdata[1]==5){
      ledrgb(CRGB::Magenta,brightness,2,5);
    }

    ledrgb_enable = 0;
  }
}

int obstacle_sensor(int trigPin, int echoPin){
  int duration,cm;
  // The sensor is triggered by a HIGH pulse of 10 or more microseconds.
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
 
  // Read the signal from the sensor: a HIGH pulse whose
  // duration is the time (in microseconds) from the sending
  // of the ping to the reception of its echo off of an object.
  pinMode(echoPin, INPUT);
  duration = pulseIn(echoPin, HIGH,timeout);
  
  // Convert the time into a distance
  cm = (duration/2) / 29.1;     // Divide by 29.1 or multiply by 0.0343
  cm_previous=cm;
      if(cm>=0) {
      return(cm);
      }

}

void requestEvent() {

  if(i2cdata[0]==1){      // #1 query all ultrasonic sensor.
   // Wire.write(detect1);  // respond with message 
    Wire.write(detect1);  // respond with message 
    Wire.write(detect2);  // respond with message 
    Wire.write(detect3);  // respond with message 
    Wire.write(detect4);  // respond with message 
    ledrgbdetect_enable = 0;
  }
  else if(i2cdata[0]==2){
    Wire.write("ZZZZ  0"); // respond with message   
  }
  else {
    Wire.write(99);
  }
}


// function that executes whenever data is received from master
//  slave device receives a transmission from a master.
void receiveEvent(int howMany) {
  int i;
   // while(Wire.available()>0){
   i2cnumbytes=howMany;
   for(i=0;i<howMany;i++){
      i2cdata[i]= Wire.read();        // read the offset register first.
      //int c = Wire.read();    
      if(howMany>1) {
      //  Serial.print(i2cdata[i]);          
      //  Serial.print(",");
      }
  }
    if(howMany>1) {
   //   Serial.print(" : WriteReg=[");
   //   Serial.print(i2cdata[0]);
   //   Serial.print("] :Num:");
   //   Serial.println(howMany);         // print the integer
      if(i2cdata[0]==1){
       // serial_enable=1;
      }
      else if(i2cdata[0]==0){
      //  serial_enable=0;
      }
      else if(i2cdata[0]==2){
        led_enable = 1;
    //    Serial.print("led");
    //    Serial.println(led_enable);
      }
      else if(i2cdata[0]==3){
        ledrgb_enable = 1;
     //   Serial.print("rgbled");
     //   Serial.println(ledrgb_enable);
      }
     else if(i2cdata[0]==4){
        motor_enable = 1;
     //   Serial.print("motorpwm");
     //   Serial.println(ledrgb_enable);
      }

    }
}

void ledrgb(CRGB color,int intensity,int minbit, int maxbit){
  if (maxbit<=NUM_LEDS && minbit <=NUM_LEDS ){
    for(minbit; minbit <= maxbit; minbit = minbit + 1) {
        leds[minbit] = color;
        }
        if(intensity<=125){
        FastLED.setBrightness(intensity);
        }
        FastLED.show();  
  }
}

void FillLEDsFromPaletteColors( uint8_t colorIndex,uint8_t brightness)
{
    
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 20;
    }
}
