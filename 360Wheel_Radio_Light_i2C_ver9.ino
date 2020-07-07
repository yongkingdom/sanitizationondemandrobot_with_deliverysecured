#include <Wire.h>
#include <FastLED.h>

#define NUM_LEDS 60
// Fly Sky Receiver Chn 1-6 in PWM mode
int pinPWM1 = 10;
int pinPWM2 = 11;
int pinPWM3 = 12;
int pinPWM4 = 13;
int pinPWM5 = A0;
int pinPWM6 = A1;

#define lowPWM 611  //611
#define highPWM 1227


// Data pin that led data will be written out over
#define DATA_PIN A2
CRGB leds[NUM_LEDS];

CRGBPalette16 currentPalette;
TBlendType    currentBlending;



#define BRAKE 0
#define CW    1
#define CCW   2
#define HALT  3

//MOTOR 1
#define MOTOR1_DIR 2
#define PWM_MOTOR1 3

//MOTOR 4
#define MOTOR4_DIR 4
#define PWM_MOTOR4 5

//MOTOR 2
#define MOTOR2_DIR 7
#define PWM_MOTOR2 6

//MOTOR 3
#define MOTOR3_DIR 8
#define PWM_MOTOR3 9

#define MOTOR_1 0
#define MOTOR_2 1
#define MOTOR_3 2
#define MOTOR_4 3

#define RGB 12

short Speed = 15;  //default motor speed
short Speed1 = 0;  
short Speed2 = 0;  
short Speed3 = 0;  
short Speed4 = 0;  

unsigned short usMotor_Status = BRAKE;

unsigned long duration1,duration2,duration3,duration4,duration5,duration6;
unsigned long chn1,chn2,chn3,chn4,chn5,chn6;
unsigned long timeout = 50000;

int i2cdata[32];              //i2cdata[0] = index 0 is the register offset. useful for selection and commands
int i2cnumbytes=32;           //number of bytes in i2c
int i=0;
int serial_enable = 0;
int rf_enable = 1;
int i2c_enable = 0;
int readRFchannel5 =0;
int readRF_enable = 0;

void setup()                         
{
  // Fly Sky Radio Receiver //
  pinMode(pinPWM1, INPUT);
  pinMode(pinPWM2, INPUT);
  pinMode(pinPWM3, INPUT);
  pinMode(pinPWM4, INPUT);
  pinMode(pinPWM5, INPUT);
  pinMode(pinPWM6, INPUT);
  
  Wire.begin(0x0A); // Start I2C on Address 0x0A
  Wire.onReceive(receiveEvent); // Receive message from RPI - write to this Slave Arduino
  Wire.onRequest(requestEvent); // Sending information back to Rpi

    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(120);
    currentPalette = RainbowColors_p;
    FillLEDsFromPaletteColors(0);
    FastLED.show();
 

  setPwmFrequency(3, 1);    //change PWM1 = 31.25kHz
  setPwmFrequency(5, 1);    //change PWM2 = 62.5kHz
  setPwmFrequency(6, 1);    //change PWM3 = 62.5kHz
  setPwmFrequency(9, 1);    //change PWM4 = 62.5kHz
   
  pinMode(MOTOR1_DIR, OUTPUT);
  pinMode(MOTOR2_DIR, OUTPUT);
  pinMode(MOTOR3_DIR, OUTPUT);
  pinMode(MOTOR4_DIR, OUTPUT);

  pinMode(PWM_MOTOR1, OUTPUT);
  pinMode(PWM_MOTOR2, OUTPUT);
  pinMode(PWM_MOTOR3, OUTPUT);
  pinMode(PWM_MOTOR4, OUTPUT);

  Serial.begin(115200);              // Initiates the serial to do the monitoring 
  Serial.println("Begin motor control");
  Serial.println(); //Print function list for user selection
  Serial.println("Enter number for control option:");
  Serial.println("5. STOP");
  Serial.println("8. FORWARD");
  Serial.println("2. REVERSE");
  Serial.println("4. LEFT");
  Serial.println("5. HALT");
  Serial.println("6. RIGHT");
  Serial.println("7. AngleForwardLeft");
  Serial.println("9. AngleForwardRight");
  Serial.println("1. AngleReverseLeft");
  Serial.println("3. AngleReverseRight");
  Serial.println("+. INCREASE SPEED");
  Serial.println("-. DECREASE SPEED");
  Serial.println("/. ROTATECCW");
  Serial.println("*. ROTATECW");
  Serial.println("[. SPINLEFT");
  Serial.println("]. SPINRIGHT");
  Serial.println();



}

void loop() 
{

  duration6 = pulseIn(pinPWM6, HIGH,timeout);
  chn6=map(duration6,lowPWM,highPWM,0,100);
  if(chn6>2500000){
    chn6=0;
  } //allows Chn6 Radio to control whether RF control enable or disable
  if ((chn6>40) && (chn6 <60)) {
    rf_enable=1;
    // Serial.println("RF enable");
  }
  else if(chn6<40){
    rf_enable=0;
    //Serial.println("RF disable");
  }
  
  if(rf_enable==1){
  
  // Fly Sky Radio receiver
  duration1 = pulseIn(pinPWM1, HIGH,timeout);
  chn1=map(duration1,lowPWM,highPWM,0,100);
  //Serial.print(duration1);
  //Serial.print(chn1);
  //Serial.print(",");
  duration2 = pulseIn(pinPWM2, HIGH,timeout);
  chn2=map(duration2,lowPWM,highPWM,0,100);
  //Serial.print(duration2);
  //Serial.print(chn2);
  //Serial.print(",");
  duration3 = pulseIn(pinPWM3, HIGH,timeout);
  chn3=map(duration3,lowPWM,highPWM,0,200);
  //Serial.print(chn3);
  //Serial.print(duration3);
  //Serial.print(",");
  duration4 = pulseIn(pinPWM4, HIGH,timeout);    //1st Joystick left-right
  chn4=map(duration4,lowPWM,highPWM,0,100);
  //Serial.print(chn4);
  //Serial.print(duration4);
  //Serial.print(",");
  duration5 = pulseIn(pinPWM5, HIGH,timeout);
  chn5=map(duration5,lowPWM,highPWM,0,100);
  if(chn5>2500000){
    chn5=0;
  }
  //Serial.print(chn5);
  //Serial.print(duration5);
  //Serial.print(",");
  duration6 = pulseIn(pinPWM6, HIGH,timeout);
  chn6=map(duration6,lowPWM,highPWM,0,100);
  if(chn6>2500000){
    chn6=0;
  }
  //Serial.println(chn6);
  //Serial.println(duration6);
   
  //Move Forward/ Backward radio control 
  if (chn2>52) {
    Speed1=Speed+chn3+chn2-52;
    Speed2=Speed+chn3+chn2-52;
    Speed3=Speed+chn3+chn2-52;
    Speed4=Speed+chn3+chn2-52;
    //Serial.println("Forward");
    usMotor_Status = CW;
    motorGo(MOTOR_1, usMotor_Status, Speed1);
    motorGo(MOTOR_2, usMotor_Status, Speed2);
    motorGo(MOTOR_3, usMotor_Status, Speed3);
    motorGo(MOTOR_4, usMotor_Status, Speed4);
  }

  else if (chn2<48) {
    Speed1=Speed+chn3+48-chn2;
    Speed2=Speed+chn3+48-chn2;
    Speed3=Speed+chn3+48-chn2;
    Speed4=Speed+chn3+48-chn2;
    //Serial.println("Backward");
    usMotor_Status = CCW;
    motorGo(MOTOR_1, usMotor_Status, Speed1);
    motorGo(MOTOR_2, usMotor_Status, Speed2);
    motorGo(MOTOR_3, usMotor_Status, Speed3);
    motorGo(MOTOR_4, usMotor_Status, Speed4);
  }
 else if (chn2>=48 && chn2 <=52) {
    Speed1=0;
    Speed2=0;
    Speed3=0;
    Speed4=0;
    //Serial.println("Stop");
    //usMotor_Status = CCW;
    motorGo(MOTOR_1, usMotor_Status, Speed1);
    motorGo(MOTOR_2, usMotor_Status, Speed2);
    motorGo(MOTOR_3, usMotor_Status, Speed3);
    motorGo(MOTOR_4, usMotor_Status, Speed4);
  }

  //Move Left/Right radio control //
     if (chn4>60) {
      Speed1=Speed+chn3+chn4-55;
      Speed2=Speed+chn3+chn4-55;
      Speed3=Speed+chn3+chn4-55;
      Speed4=Speed+chn3+chn4-55;
      //Serial.println("Left");
      usMotor_Status = CW;
      motorGo(MOTOR_1, usMotor_Status, Speed1);
      usMotor_Status = CCW;
      motorGo(MOTOR_2, usMotor_Status, Speed2);
      usMotor_Status = CW;
      motorGo(MOTOR_3, usMotor_Status, Speed3);
      usMotor_Status = CCW;
      motorGo(MOTOR_4, usMotor_Status, Speed4);
    }
    else if (chn4<40) {
      Speed1=Speed+chn3+45-chn4;
      Speed2=Speed+chn3+45-chn4;
      Speed3=Speed+chn3+45-chn4;
      Speed4=Speed+chn3+45-chn4;
      //Serial.println("Right");
      usMotor_Status = CCW;
      motorGo(MOTOR_1, usMotor_Status, Speed1);
      usMotor_Status = CW;
      motorGo(MOTOR_2, usMotor_Status, Speed2);
      usMotor_Status = CCW;
      motorGo(MOTOR_3, usMotor_Status, Speed3);
      usMotor_Status = CW;
      motorGo(MOTOR_4, usMotor_Status, Speed4);
    }
//    else if (chn4>=48 && chn4 <=52) {
//    Speed1=0;
//    Speed2=0;
//    Speed3=0;
//    Speed4=0;
//    Serial.println("Stop");
//    //usMotor_Status = CCW;
//    motorGo(MOTOR_1, usMotor_Status, Speed1);
//    motorGo(MOTOR_2, usMotor_Status, Speed2);
//    motorGo(MOTOR_3, usMotor_Status, Speed3);
//    motorGo(MOTOR_4, usMotor_Status, Speed4);
//  }  
    
    //Spin CW/CCW radio control //
     if (chn1>52) {
      Speed1=Speed+chn3+chn1-52;
      Speed2=Speed+chn3+chn1-52;
      Speed3=Speed+chn3+chn1-52;
      Speed4=Speed+chn3+chn1-52;
      //Serial.println("Rotate CW");
      usMotor_Status = CW;
      motorGo(MOTOR_1, usMotor_Status, Speed1);
      usMotor_Status = CW;
      motorGo(MOTOR_2, usMotor_Status, Speed2);
      usMotor_Status = CCW;
      motorGo(MOTOR_3, usMotor_Status, Speed3);
      usMotor_Status = CCW;
      motorGo(MOTOR_4, usMotor_Status, Speed4);
     }
     else if(chn1<48){
      Speed1=Speed+chn3+48-chn1;
      Speed2=Speed+chn3+48-chn1;
      Speed3=Speed+chn3+48-chn1;
      Speed4=Speed+chn3+48-chn1;
      //Serial.println("Rotate CCW");
      usMotor_Status = CCW;
      motorGo(MOTOR_1, usMotor_Status, Speed1);
      usMotor_Status = CCW;
      motorGo(MOTOR_2, usMotor_Status, Speed2);
      usMotor_Status = CW;
      motorGo(MOTOR_3, usMotor_Status, Speed3);
      usMotor_Status = CW;
      motorGo(MOTOR_4, usMotor_Status, Speed4);
     }
     
  //******************************************************//
  }

  if(readRF_enable==1){
      duration5 = pulseIn(pinPWM5, HIGH,timeout);
      chn5=map(duration5,lowPWM,highPWM,0,100);
      readRFchannel5=chn5;
      //Serial.println("Channel5");
      readRF_enable=0;
  }
  
  if(serial_enable==1)
  {
  //// serial data read from Python /////
  char user_input;   
  while(Serial.available())
  {
    user_input = Serial.read(); //Read user input and trigger appropriate function
    
    if (user_input =='5')
    {
      Stop();
      //Halt();
    }
    else if(user_input =='8')
    {
      Forward();
      
    }
    else if(user_input =='2')
    {
      Reverse();
    }
    else if(user_input =='4')
    {
      Left();
    }
    else if(user_input =='6')
    {
      Right();
    }
    else if(user_input =='/')
    {
      RotateCW();
    }
    else if(user_input =='*')
    {
      RotateCCW();
    }
    else if(user_input =='7')
    {
      AngleForwardLeft();
    }
    else if(user_input =='9')
    {
      AngleForwardRight();
    }
    else if(user_input =='1')
    {
      AngleReverseLeft();
    }
    else if(user_input =='3')
    {
      AngleReverseRight();
    }
    else if(user_input =='+')
    {
      IncreaseSpeed();
    }
    else if(user_input =='-')
    {
      DecreaseSpeed();
    }
    else if(user_input =='[')
    {
      ForwardSpinLeft();
    }
    else if(user_input ==']')
    {
      ForwardSpinRight();
    }
    else if(user_input =='o')
    {
      ReverseSpinLeft();
    }
    else if(user_input =='p')
    {
      ReverseSpinRight();
    }
    else if(user_input =='5')
    {
      Halt();
    }
    else if(user_input =='g')
    {
      for(int Led = 0; Led < 60; Led = Led + 1) {
      leds[Led] = CRGB::Green;
      FastLED.show();  
      }
    } 
    else if(user_input =='r')
    {
      for(int Led = 0; Led < 60; Led = Led + 1) {
      leds[Led] = CRGB::Red;
      FastLED.show();  
      }
    } 
    else if(user_input =='b')
    {
      for(int Led = 0; Led < 60; Led = Led + 1) {
      leds[Led] = CRGB::Blue;
      FastLED.show();  
      }
    }     
    else if(user_input =='w')
    {
      for(int Led = 0; Led < 60; Led = Led + 1) {
      leds[Led] = CRGB::White;
      FastLED.show();  
      }
    }     
    else if(user_input =='x')
    {
      for(int Led = 0; Led < 60; Led = Led + 1) {
      leds[Led] = CRGB::Black;
      FastLED.show();  
      }
    }     
    else if(user_input =='t')
    {
      FastLED.setBrightness(255);
      for(int Led = 0; Led < 60; Led = Led + 1) {
      leds[Led+1] = CRGB::White;
      leds[Led] = CRGB::Black;       
      FastLED.show();
      }
      FastLED.setBrightness(120);
    }     
    
      else if(user_input =='u')
      {
      FastLED.setBrightness(255);
      }     
      else if(user_input =='j')
      {
      FastLED.setBrightness(120);
      }     
        
    else
      {
      //Serial.println("Invalid option entered.");
      }
      
  }
  }

 if(i2c_enable==1) {
   Serial.println("I2C enable");
   
  /// i2c data read from raspberry Pi ///
          if (i2cdata[1] ==5)
          {
          Stopi(i2cdata[2]);
          }
        else if(i2cdata[1] ==2)
          {
          Forwardi(i2cdata[2]);
          }
        else if(i2cdata[1] ==8)
          {
          Reversei(i2cdata[2]);
          }
        else if(i2cdata[1] ==10)
          {
          Lefti(i2cdata[2]);
          }
        else if(i2cdata[1] ==11)
          {
          Righti(i2cdata[2]);
          }
        else if(i2cdata[1] ==4)
          {
          RotateCWi(i2cdata[2]);
          }
        else if(i2cdata[1] ==6)
          {
          RotateCCWi(i2cdata[2]);
          }
        else if(i2cdata[1] ==7)
          {
          AngleForwardLeft();
          }
        else if(i2cdata[1] ==9)
          {
          AngleForwardRight();
          }
        else if(i2cdata[1] ==1)
          {
          AngleReverseLeft();
          }
        else if(i2cdata[1] ==3)
          {
          AngleReverseRight();
          }
        else if(i2cdata[1] ==13)
          {
          IncreaseSpeed();
          //i2cdata[1]=0;
          }
        else if(i2cdata[1] ==12)
          {
          DecreaseSpeed();
          //i2cdata[1]=0;
          }
        else if(i2cdata[1] ==14)
          {
          Halt();
          }
        else if(i2cdata[1] ==15)
          {
          ForwardSpinLefti(i2cdata[2]);
          }
        else if(i2cdata[1] ==16)
          {
          ForwardSpinRighti(i2cdata[2]);
          }
        else if(i2cdata[1] ==17)
          {
          ReverseSpinLefti(i2cdata[2]);
          }
        else if(i2cdata[1] ==18)
          {
          ReverseSpinRighti(i2cdata[2]);
          }
        else if(i2cdata[1] ==50)
          {
            for(int Led = 0; Led < 60; Led = Led + 1) {
            leds[Led] = CRGB::Green;
            FastLED.show();  
            }
          } 
        else if(i2cdata[1] ==51)
          {
           for(int Led = 0; Led < 60; Led = Led + 1) {
           leds[Led] = CRGB::Red;
           FastLED.show();  
           }
          }  
        else if(i2cdata[1] ==52)
          {
            for(int Led = 0; Led < 60; Led = Led + 1) {
            leds[Led] = CRGB::Blue;
            FastLED.show();  
            }
          }     
        else if(i2cdata[1] ==53)
          {
            for(int Led = 0; Led < 60; Led = Led + 1) {
            leds[Led] = CRGB::White;
            FastLED.show();  
             }
          }     
        else if(i2cdata[1] ==54)
          {
            for(int Led = 0; Led < 60; Led = Led + 1) {
            leds[Led] = CRGB::Black;
            FastLED.show();  
             }
          }     
        else if(i2cdata[1] ==55)
          {
            FastLED.setBrightness(255);
            for(int Led = 0; Led < 60; Led = Led + 1) {
            leds[Led+1] = CRGB::White;
            leds[Led] = CRGB::Black;  
            FastLED.show();  
             }
           FastLED.setBrightness(120);
          }     
        else if(i2cdata[1] ==60)
          {
          FastLED.setBrightness(120);
          }
        else if(i2cdata[1] ==61)
          {
          FastLED.setBrightness(255);
          }      
        
        else
        {
         //Halt();
        }
  i2c_enable=0;
 }



}



//////////////////////////// Functions /////////////////////////////
void Stop()
{
  int i;
  int tempspeed;
  Serial.println("Stop");
  tempspeed=Speed;
  usMotor_Status = BRAKE;
  for(i=0;i<=200;i++){
    if(tempspeed>0) {
      tempspeed = tempspeed-1;
      delay(10);
    }
    else {
      tempspeed = 0;
      i=200;
    }
    if(Speed1!=0) {
      motorGo(MOTOR_1, usMotor_Status, tempspeed);        
    }
    if(Speed2!=0) {
      motorGo(MOTOR_2, usMotor_Status, tempspeed);        
    }
    if(Speed3!=0) {
      motorGo(MOTOR_3, usMotor_Status, tempspeed);        
    }
    if(Speed4!=0) {
      motorGo(MOTOR_4, usMotor_Status, tempspeed);        
    }

  }

  
  Speed1=0;
  Speed2=0;
  Speed3=0;
  Speed4=0;

}

void Stopi(int deceleration)
{
  int i;
  int tempspeed;
  Serial.println("Stopi:");
  tempspeed=Speed;
  usMotor_Status = BRAKE;
  for(i=0;i<=Speed;i++){
    if(tempspeed>0) {
      tempspeed = tempspeed-1;
      delay(deceleration*10);
      }
   else {
      tempspeed = 0;
      i=Speed;
    }
   // Serial.print(deceleration);
   // Serial.print(",");
   // Serial.println(tempspeed);
    
    if(Speed1!=0) {
      motorGo(MOTOR_1, usMotor_Status, tempspeed);        
    }
    if(Speed2!=0) {
      motorGo(MOTOR_2, usMotor_Status, tempspeed);        
    }
    if(Speed3!=0) {
      motorGo(MOTOR_3, usMotor_Status, tempspeed);        
    }
    if(Speed4!=0) {
      motorGo(MOTOR_4, usMotor_Status, tempspeed);        
    }

  }

  
  Speed1=0;
  Speed2=0;
  Speed3=0;
  Speed4=0;

}

void Halt()
{
  Serial.println("Halt");
  usMotor_Status = HALT;
  motorGo(MOTOR_1, usMotor_Status, 0);
  motorGo(MOTOR_2, usMotor_Status, 0);
  motorGo(MOTOR_3, usMotor_Status, 0);
  motorGo(MOTOR_4, usMotor_Status, 0);
  Speed1=0;
  Speed2=0;
  Speed3=0;
  Speed4=0;
}

void Forward()
{
  
  Speed1=Speed;
  Speed2=Speed;
  Speed3=Speed;
  Speed4=Speed;
  Serial.println("Forward");
  usMotor_Status = CW;
  motorGo(MOTOR_1, usMotor_Status, Speed1);
  motorGo(MOTOR_2, usMotor_Status, Speed2);
  motorGo(MOTOR_3, usMotor_Status, Speed3);
  motorGo(MOTOR_4, usMotor_Status, Speed4);
}

void Forwardi(int motorspeed)
{
  
  Speed=motorspeed;
  Speed1=Speed;
  Speed2=Speed;
  Speed3=Speed;
  Speed4=Speed;
  Serial.print("Forward:");
  Serial.println(Speed);
  usMotor_Status = CW;
  motorGo(MOTOR_1, usMotor_Status, Speed1);
  motorGo(MOTOR_2, usMotor_Status, Speed2);
  motorGo(MOTOR_3, usMotor_Status, Speed3);
  motorGo(MOTOR_4, usMotor_Status, Speed4);
  
}

void Reverse()
{
  
  Speed1=Speed;
  Speed2=Speed;
  Speed3=Speed;
  Speed4=Speed;
  Serial.println("Reverse");
  usMotor_Status = CCW;
  motorGo(MOTOR_1, usMotor_Status, Speed1);
  motorGo(MOTOR_2, usMotor_Status, Speed2);
  motorGo(MOTOR_3, usMotor_Status, Speed3);
  motorGo(MOTOR_4, usMotor_Status, Speed4);
}

void Reversei(int motorspeed)
{
  
  Speed=motorspeed;
  Speed1=Speed;
  Speed2=Speed;
  Speed3=Speed;
  Speed4=Speed;
  Serial.print("Reverse:");
  Serial.println(Speed);
  usMotor_Status = CCW;
  motorGo(MOTOR_1, usMotor_Status, Speed1);
  motorGo(MOTOR_2, usMotor_Status, Speed2);
  motorGo(MOTOR_3, usMotor_Status, Speed3);
  motorGo(MOTOR_4, usMotor_Status, Speed4);
}

void Left()
{
  
  Speed1=Speed;
  Speed2=Speed;
  Speed3=Speed;
  Speed4=Speed;
  Serial.println("Left");
  usMotor_Status = CCW;
  motorGo(MOTOR_1, usMotor_Status, Speed1);
  usMotor_Status = CW;
  motorGo(MOTOR_2, usMotor_Status, Speed2);
  usMotor_Status = CCW;
  motorGo(MOTOR_3, usMotor_Status, Speed3);
  usMotor_Status = CW;
  motorGo(MOTOR_4, usMotor_Status, Speed4);
}

void Lefti(int motorspeed)
{
  Speed=motorspeed;
  Speed1=Speed;
  Speed2=Speed;
  Speed3=Speed;
  Speed4=Speed;
  Serial.println("Left");
  usMotor_Status = CCW;
  motorGo(MOTOR_1, usMotor_Status, Speed1);
  usMotor_Status = CW;
  motorGo(MOTOR_2, usMotor_Status, Speed2);
  usMotor_Status = CCW;
  motorGo(MOTOR_3, usMotor_Status, Speed3);
  usMotor_Status = CW;
  motorGo(MOTOR_4, usMotor_Status, Speed4);
}

void Right()
{
  Speed1=Speed;
  Speed2=Speed;
  Speed3=Speed;
  Speed4=Speed;
  Serial.println("Right");
  usMotor_Status = CW;
  motorGo(MOTOR_1, usMotor_Status, Speed1);
  usMotor_Status = CCW;
  motorGo(MOTOR_2, usMotor_Status, Speed2);
  usMotor_Status = CW;
  motorGo(MOTOR_3, usMotor_Status, Speed3);
  usMotor_Status = CCW;
  motorGo(MOTOR_4, usMotor_Status, Speed4);
}
void Righti(int motorspeed)
{
  Speed=motorspeed;
  Speed1=Speed;
  Speed2=Speed;
  Speed3=Speed;
  Speed4=Speed;
  Serial.println("Right");
  usMotor_Status = CW;
  motorGo(MOTOR_1, usMotor_Status, Speed1);
  usMotor_Status = CCW;
  motorGo(MOTOR_2, usMotor_Status, Speed2);
  usMotor_Status = CW;
  motorGo(MOTOR_3, usMotor_Status, Speed3);
  usMotor_Status = CCW;
  motorGo(MOTOR_4, usMotor_Status, Speed4);
}

void RotateCW()
{
  
  Speed1=Speed;
  Speed2=Speed;
  Speed3=Speed;
  Speed4=Speed;
  Serial.println("Rotate CW");
  usMotor_Status = CW;
  motorGo(MOTOR_1, usMotor_Status, Speed1);
  usMotor_Status = CW;
  motorGo(MOTOR_2, usMotor_Status, Speed2);
  usMotor_Status = CCW;
  motorGo(MOTOR_3, usMotor_Status, Speed3);
  usMotor_Status = CCW;
  motorGo(MOTOR_4, usMotor_Status, Speed4);
}

void RotateCWi(int motorspeed)
{
  Speed = motorspeed;
  Speed1=Speed;
  Speed2=Speed;
  Speed3=Speed;
  Speed4=Speed;
  Serial.println("Rotate CW");
  usMotor_Status = CW;
  motorGo(MOTOR_1, usMotor_Status, Speed1);
  usMotor_Status = CW;
  motorGo(MOTOR_2, usMotor_Status, Speed2);
  usMotor_Status = CCW;
  motorGo(MOTOR_3, usMotor_Status, Speed3);
  usMotor_Status = CCW;
  motorGo(MOTOR_4, usMotor_Status, Speed4);
}
void RotateCCW()
{
  Speed1=Speed;
  Speed2=Speed;
  Speed3=Speed;
  Speed4=Speed;
  Serial.println("Rotate CCW");
  usMotor_Status = CCW;
  motorGo(MOTOR_1, usMotor_Status, Speed1);
  usMotor_Status = CCW;
  motorGo(MOTOR_2, usMotor_Status, Speed2);
  usMotor_Status = CW;
  motorGo(MOTOR_3, usMotor_Status, Speed3);
  usMotor_Status = CW;
  motorGo(MOTOR_4, usMotor_Status, Speed4);
}

void RotateCCWi(int motorspeed)
{
  Speed = motorspeed;
  Speed1=Speed;
  Speed2=Speed;
  Speed3=Speed;
  Speed4=Speed;
  Serial.println("Rotate CCW");
  usMotor_Status = CCW;
  motorGo(MOTOR_1, usMotor_Status, Speed1);
  usMotor_Status = CCW;
  motorGo(MOTOR_2, usMotor_Status, Speed2);
  usMotor_Status = CW;
  motorGo(MOTOR_3, usMotor_Status, Speed3);
  usMotor_Status = CW;
  motorGo(MOTOR_4, usMotor_Status, Speed4);
}

void AngleReverseLeft()
{
  Speed1=Speed;
  Speed2=0;
  Speed3=Speed;
  Speed4=0;
  Serial.println("AngleReverseLeft");
  usMotor_Status = CCW;
  motorGo(MOTOR_1, usMotor_Status, Speed1);
  usMotor_Status = CW;
  motorGo(MOTOR_2, usMotor_Status, Speed2);
  usMotor_Status = CCW;
  motorGo(MOTOR_3, usMotor_Status, Speed3);
  usMotor_Status = CW;
  motorGo(MOTOR_4, usMotor_Status, Speed4);
}

void AngleReverseRight()
{
  Speed1=0;
  Speed2=Speed;
  Speed3=0;
  Speed4=Speed;
  Serial.println("AngleReverseRight");
  usMotor_Status = CCW;
  motorGo(MOTOR_1, usMotor_Status, Speed1);
  usMotor_Status = CCW;
  motorGo(MOTOR_2, usMotor_Status, Speed2);
  usMotor_Status = CW;
  motorGo(MOTOR_3, usMotor_Status, Speed3);
  usMotor_Status = CCW;
  motorGo(MOTOR_4, usMotor_Status, Speed4);
}
void AngleForwardLeft()
{
  Speed1=0;
  Speed2=Speed;
  Speed3=0;
  Speed4=Speed;
  Serial.println("AngleForwardLeft");
  usMotor_Status = CCW;
  motorGo(MOTOR_1, usMotor_Status, Speed1);
  usMotor_Status = CW;
  motorGo(MOTOR_2, usMotor_Status, Speed2);
  usMotor_Status = CCW;
  motorGo(MOTOR_3, usMotor_Status, Speed3);
  usMotor_Status = CW;
  motorGo(MOTOR_4, usMotor_Status, Speed4);
}
void AngleForwardRight()
{
  Speed1=Speed;
  Speed2=0;
  Speed3=Speed;
  Speed4=0;

  Serial.println("AngleForwardRight");
  usMotor_Status = CW;
  motorGo(MOTOR_1, usMotor_Status, Speed1);
  usMotor_Status = CW;
  motorGo(MOTOR_2, usMotor_Status, Speed2);
  usMotor_Status = CW;
  motorGo(MOTOR_3, usMotor_Status, Speed3);
  usMotor_Status = CW;
  motorGo(MOTOR_4, usMotor_Status, Speed4);
}

void ForwardSpinLeft()
{
  
  Speed1=Speed;
  Speed2=Speed;
  Speed3=0;
  Speed4=0;
  Serial.println("ForwardSpinLeft");
  usMotor_Status = CW;
  motorGo(MOTOR_1, usMotor_Status, Speed1);
  motorGo(MOTOR_2, usMotor_Status, Speed2);
  motorGo(MOTOR_3, usMotor_Status, Speed3);
  motorGo(MOTOR_4, usMotor_Status, Speed4);
}

void ForwardSpinLefti(int motorspeed)
{
  Speed = motorspeed;
  Speed1=0;
  Speed2=Speed;
  Speed3=0;
  Speed4=Speed;
  Serial.println("ForwardSpinLeft");
  usMotor_Status = CCW;
  motorGo(MOTOR_1, usMotor_Status, Speed1);
  motorGo(MOTOR_2, usMotor_Status, Speed2);
  motorGo(MOTOR_3, usMotor_Status, Speed3);
  motorGo(MOTOR_4, usMotor_Status, Speed4);
}


void ForwardSpinRight()
{
  
  Speed1=0;
  Speed2=0;
  Speed3=Speed;
  Speed4=Speed;
  Serial.println("ForwardSpinRight");
  usMotor_Status = CW;
  motorGo(MOTOR_1, usMotor_Status, Speed1);
  motorGo(MOTOR_2, usMotor_Status, Speed2);
  motorGo(MOTOR_3, usMotor_Status, Speed3);
  motorGo(MOTOR_4, usMotor_Status, Speed4);
}

void ForwardSpinRighti(int motorspeed)
{
  Speed = motorspeed;
  Speed1=Speed;
  Speed2=0;
  Speed3=Speed;
  Speed4=0;
  Serial.println("ForwardSpinRight");
  usMotor_Status = CCW;
  motorGo(MOTOR_1, usMotor_Status, Speed1);
  motorGo(MOTOR_2, usMotor_Status, Speed2);
  motorGo(MOTOR_3, usMotor_Status, Speed3);
  motorGo(MOTOR_4, usMotor_Status, Speed4);
}

void ReverseSpinLeft()
{
  
  Speed1=Speed;
  Speed2=Speed;
  Speed3=0;
  Speed4=0;
  Serial.println("ReverseSpinLeft");
  usMotor_Status = CCW;
  motorGo(MOTOR_1, usMotor_Status, Speed1);
  motorGo(MOTOR_2, usMotor_Status, Speed2);
  motorGo(MOTOR_3, usMotor_Status, Speed3);
  motorGo(MOTOR_4, usMotor_Status, Speed4);
}

void ReverseSpinLefti(int motorspeed)
{
  Speed = motorspeed;
  Speed1=0;
  Speed2=Speed;
  Speed3=0;
  Speed4=Speed;
  Serial.println("ReverseSpinLeft");
  usMotor_Status = CW;
  motorGo(MOTOR_1, usMotor_Status, Speed1);
  motorGo(MOTOR_2, usMotor_Status, Speed2);
  motorGo(MOTOR_3, usMotor_Status, Speed3);
  motorGo(MOTOR_4, usMotor_Status, Speed4);
}
void ReverseSpinRight()
{
  
  Speed1=0;
  Speed2=0;
  Speed3=Speed;
  Speed4=Speed;
  Serial.println("ReverseSpinRight");
  usMotor_Status = CCW;
  motorGo(MOTOR_1, usMotor_Status, Speed1);
  motorGo(MOTOR_2, usMotor_Status, Speed2);
  motorGo(MOTOR_3, usMotor_Status, Speed3);
  motorGo(MOTOR_4, usMotor_Status, Speed4);
}

void ReverseSpinRighti(int motorspeed)
{
  Speed = motorspeed;
  Speed1=Speed;
  Speed2=0;
  Speed3=Speed;
  Speed4=0;
  Serial.println("ReverseSpinRight");
  usMotor_Status = CW;
  motorGo(MOTOR_1, usMotor_Status, Speed1);
  motorGo(MOTOR_2, usMotor_Status, Speed2);
  motorGo(MOTOR_3, usMotor_Status, Speed3);
  motorGo(MOTOR_4, usMotor_Status, Speed4);
}

void IncreaseSpeed()
{
  Speed = Speed + 2;
  if(Speed > 255)
  {
    Speed = 255;  
  }
  
  Serial.print("Speed +: ");
  Serial.println(Speed);
  if(Speed1!=0)
    {
      Speed1=Speed;
      analogWrite(PWM_MOTOR1,Speed1);
    }
  if(Speed2!=0)
    {
      Speed2=Speed;
      analogWrite(PWM_MOTOR2,Speed2);
    }
  if(Speed3!=0)
    {
      Speed3=Speed;
      analogWrite(PWM_MOTOR3,Speed3);
    }
  if(Speed4!=0)
    {
      Speed4=Speed;
      analogWrite(PWM_MOTOR4,Speed4);
    }
  
  }

void DecreaseSpeed()
{
  Speed = Speed - 2;
  if(Speed < 0)
  {
    Speed = 0;  
  }
  
  Serial.print("Speed -: ");
  Serial.println(Speed);
    if(Speed1!=0)
    {
      Speed1=Speed;
      analogWrite(PWM_MOTOR1,Speed1);
    }
  if(Speed2!=0)
    {
      Speed2=Speed;
      analogWrite(PWM_MOTOR2,Speed2);
    }
  if(Speed3!=0)
    {
      Speed3=Speed;
      analogWrite(PWM_MOTOR3,Speed3);
    }
  if(Speed4!=0)
    {
      Speed4=Speed;
      analogWrite(PWM_MOTOR4,Speed4);
    }


}
void motorGo(uint8_t motor, uint8_t direct, uint8_t pwm)         //Function that controls the variables: motor(0 ou 1), direction (cw ou ccw) e pwm (entra 0 e 255);
{
  if(motor == MOTOR_1)
  {
    analogWrite(PWM_MOTOR1, pwm); 
    if(direct == CW)
    {
      digitalWrite(MOTOR1_DIR, HIGH); 
    }
    else if(direct == CCW)
    {
      digitalWrite(MOTOR1_DIR, LOW);
    }
    else if(direct == HALT)
    {
      digitalWrite(PWM_MOTOR1, LOW);
    }
    

    
  }
  else if(motor == MOTOR_2)
  {
    analogWrite(PWM_MOTOR2, pwm);
    if(direct == CW)
    {
      digitalWrite(MOTOR2_DIR, HIGH);
    }
    else if(direct == CCW)
    {
      digitalWrite(MOTOR2_DIR, LOW);
    }
    else if(direct == HALT)
    {
     digitalWrite(PWM_MOTOR2, LOW);
    }
    
   
  }
  else if(motor == MOTOR_3)
  {
    analogWrite(PWM_MOTOR3, pwm);
    if(direct == CW)
    {
      digitalWrite(MOTOR3_DIR, HIGH);
    }
    else if(direct == CCW)
    {
      digitalWrite(MOTOR3_DIR, LOW);
    }
    else if(direct == HALT)
    {
      digitalWrite(PWM_MOTOR3, LOW);
    }
    
   
  }
  else if(motor == MOTOR_4)
  {
   
    analogWrite(PWM_MOTOR4, pwm);
    if(direct == CW)
    {
     digitalWrite(MOTOR4_DIR, HIGH);
    }
    else if(direct == CCW)
    {
      digitalWrite(MOTOR4_DIR, LOW);
    }
    else if(direct == HALT)
    {
    digitalWrite(PWM_MOTOR4, LOW);
    }
   
  }
}

void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x07; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}

void requestEvent() {

  if(i2cdata[0]==1){      // #1 query all ultrasonic sensor.
    Wire.write(readRFchannel5);  // respond with message 
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
     //   Serial.print(i2cdata[i]);          
     //   Serial.print(",");
      }
  }
    if(howMany>1) {
    //  Serial.print(" : Write_reg=[");
    //  Serial.print(i2cdata[0]);
    //  Serial.print("] :Num:");
    //  Serial.println(howMany);         // print the integer
      if(i2cdata[0]==1){
        i2c_enable=1;
      }
      else if(i2cdata[0]==0){
        i2c_enable=0;
      }
      else if(i2cdata[0]==2){
        rf_enable=0;
      }
      else if(i2cdata[0]==3){
        rf_enable=1;
      }
      else if(i2cdata[0]==4){
        readRF_enable=1;
      }

    }
}


void FillLEDsFromPaletteColors( uint8_t colorIndex)
{
    uint8_t brightness = 255;
    
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( currentPalette, colorIndex, brightness, currentBlending);
        colorIndex += 3;
    }
}
