#include <Servo.h>
#include <Wire.h>
Servo servo1;
int pwmservo = 6;
int endswitch = 13;

int motorpwm = 5;
int IN3 = 4;
int IN4 = 7;

int pumppwm = 11;
int IN1 = 10 ;
int IN2 = 8;

int encoder1 = 2;
int encoder2 = 3;
int mister = 9;

int encodercount =0;
int bigcount = 0;
int positionstate=  0;
#define up  1
#define down  0
#define halt  3
#define encodermax 3587
#define encodermin 0

int i2cdata[32];              //i2cdata[0] = index 0 is the register offset. useful for selection and commands
int i2cnumbytes=32;           //number of bytes in i2c
int i2c_robotarm_enable = 0;
int servoangle =0;

void setup() {
  
  Wire.begin(0x9); // Start I2C on Address 0x09
  Wire.onReceive(receiveEvent); // Receive message from RPI - write to this Slave Arduino
  Wire.onRequest(requestEvent); // Sending information back to the RPI
  
  Serial.begin(115200);
  Serial.println("RobotArmLiftPresser");

  pinMode(mister,INPUT);     //set Input to Mister gpio as high impedance input
  delay(1000);
  pinMode(mister,OUTPUT);     //set Output to Mister gpio as low impedance output
  //digitalWrite(mister,HIGH);
  
//  setPwmFrequency(3, 1);    //change PWM1 = 31.25kHz
  setPwmFrequency(5, 1);    //change PWM2 = 62.5kHz
//  setPwmFrequency(6, 1);    //change PWM3 = 62.5kHz
//  setPwmFrequency(9, 1);    //change PWM4 = 62.5kHz

  /*   for(int i=0;i<=10;i++){    // 1 series of pulse to turn on Mister. 
    digitalWrite(mister,HIGH);
    delay(1);
    digitalWrite(mister,LOW);
    delay(1);
    }
   digitalWrite(mister,HIGH);
  */
  
  servo1.attach(pwmservo);
  servo1.write(95);
  
  pinMode(endswitch,INPUT_PULLUP);
  pinMode(motorpwm,OUTPUT);   //linear dc motor
  pinMode(IN3,OUTPUT);
  pinMode(IN4,OUTPUT);
  analogWrite(motorpwm,10);

  pinMode(pumppwm,OUTPUT);      //water pump
  pinMode(IN1,OUTPUT);
  pinMode(IN2,OUTPUT);
  //analogWrite(pumppwm,11);      // min PWM to turn on Pump
  //  while(1){
  //waterpump(1,15);
  //Serial.println("On Pump");
  //delay(300000);
  //waterpump(0,0);
  //Serial.println("Off Pump");
  //delay(300000);   
  //}
  
  pinMode(encoder1,INPUT);
  pinMode(encoder2,INPUT);
  attachInterrupt(digitalPinToInterrupt(encoder1),count,CHANGE);
  Serial.println();
  Serial.println("Homing");

  while(digitalRead(endswitch)==1)
    {
    motor(down,140);
    delay(150);
    motor(halt,0);
    delay(50);
    }
  
  motor(halt,0);
  delay(20000);

  encodercount=0;             //reset encoder. encoder takes 3587 counts end to end
  Serial.println("Reached");   
  Serial.println("Motor Centered");   
  //motorposition(encodermax/2,190);
  delay(10000);
  positionstate= 100;
  servo1.write(5);
 // mist_on();
 // Serial.println("Mist Primed");
}

void loop() {


int endstop;
int incomingByte;
   //i2cdata[0] = 4 // write to position and servo with i2cdata[1] + i2cdata[2] and servo angle = i2cdata[3]
   //i2cdata[0] = 5 // write only to servo with i2cdata[1] only.
   //i2cdata[0] = 6 // write for home linear arm and reset encoder and positionstate
   //i2cdata[0] = 7 // write to return back encodercount value to RPI for reading
   //i2cdata[0] = 8 // write to turn on /led/off mist.
   //i2cdata[0] = 9 // write to turn on /off waterpump.
   
    
    if (i2cdata[0]==4){         // i2c command to write to linear servo position
    positionstate = i2cdata[1] + (i2cdata[2] << 8);      //pass master's i2c data for new positioning from double byte into 16bit integer
    if(i2cdata[3]<=180){
      servo1.write(i2cdata[3]);
     i2cdata[0]=0;
     }
    }
    
    if(i2cdata[0]==5){          // i2c command to write to servo position
      servo1.write(i2cdata[1]);
      i2cdata[0]=0;
     }

    if(i2cdata[0]==6){            // i2c command to reset home position
        Serial.println("Homing...");
        servo1.write(5);
        while(digitalRead(endswitch)==1)
            {
            motor(down,140);
            delay(150);
            motor(halt,0);
            delay(50);
            }
       motor(halt,0);
       encodercount=0;              // reset encoder
       positionstate=1700;          // reset positioinstate slightly higher
       delay(2000);
       Serial.println("Home");
       i2cdata[0]=0;                //clear data
    }

    if(i2cdata[0]==8){              // i2c command to turn on Mist_on
      mist_on();
      i2cdata[0]=0;                //clear data
    }

    if(i2cdata[0]==9){                  // i2c command to turn on waterpump
      waterpump(i2cdata[1],i2cdata[2]);
      i2cdata[0]=0;                    //clear data
    }

    if(encodercount>=positionstate+50 || encodercount <=positionstate-50){   //Hysteris to eliminate whining noise during lock position.
      motorposition(positionstate,220);                                       // move to position.
      }
    else if (encodercount>=positionstate+10 || encodercount <=positionstate-10){   //Hysteris to eliminate whining noise during lock position.
      motorposition(positionstate,190);                                       // move to position.
      }

  
    if(Serial.available()>0){       //for debugging purposes via serial monitor.
        incomingByte=Serial.read();
        if(incomingByte=='8')
           {
            positionstate=positionstate+50;
            if(positionstate>=encodermax-50){
            positionstate=encodermax-50;
           }
      }
    else if(incomingByte=='2'){   
        positionstate=positionstate-50;
        if(positionstate<=encodermin+50)
           {
           positionstate=encodermin+50;
             }
        }
    
   else if(incomingByte=='m'){    // on /led/off/ mist
        mist_on();
        Serial.println("Mister");
      }
   
   else if(incomingByte=='w'){    //on min water pump 
        waterpump(1,15);
      }
   else if(incomingByte=='s'){    //off water pump
        waterpump(0,0);
      }
   else if(incomingByte=='e'){    //on max water pump
        waterpump(1,200);
      }
    }
  
}

///////////////////////// Subroutines ////////////////////////


void count(){                                   // ISR routine for interrupt pin 2 encoder signal


  if(digitalRead(encoder1)!=digitalRead(encoder2)) //quadrature encoder for detecting motor direction.
    {
    encodercount++;                             //increment encoder value if moving up
    }
  else
    {
    encodercount--;                             //decrement encoder value if moving down
    }
  
}


void motor(int direction,int velocity){
  if(direction == up)
  {
     analogWrite(motorpwm,velocity);
     digitalWrite(IN3,HIGH);
     digitalWrite(IN4,LOW);
  }
  else if( direction == down)
  {
     analogWrite(motorpwm,velocity);
     digitalWrite(IN3,LOW);
     digitalWrite(IN4,HIGH);
  }
  else if( direction == halt)
  {
     analogWrite(motorpwm,0);
     digitalWrite(IN3,LOW);
     digitalWrite(IN4,LOW);
  }

}

void motorposition(int encoderposition,int velocity){
   
   if (encodercount<encoderposition+5)
     {
      motor(up,velocity);
      while (encodercount<=encoderposition){
      Serial.println(encodercount);
      }
     }
   else if(encodercount>encoderposition-5)
    {
     motor(down,140);
     while (encodercount>encoderposition){
     Serial.println(encodercount);
     
      }
     }
   motor(halt,0);
   
   //Serial.println("Halt");
   
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

void mist_on(){
  pinMode(mister,INPUT);     //set Input to Mister gpio as high impedance input
  delay(3000);
  pinMode(mister,OUTPUT);     //set Output to Mister gpio as low impedance output
  digitalWrite(mister,LOW);
  delay(3000);
  
   for(int i=0;i<=100;i++){
      digitalWrite(mister,HIGH);  //generate simulated finger touch pulses.
      delay(1);
      digitalWrite(mister,LOW);
      delay(1);
    }
    digitalWrite(mister,HIGH);    //set high to remain last touch pulses.
 
  
}

void waterpump(int on_off, int speed){

 if(on_off==1){
    analogWrite(pumppwm,speed);      // PWM to turn on Pump
    digitalWrite(IN1,HIGH);
    digitalWrite(IN2,LOW);
    Serial.println("WaterPump On");
   }
 else if(on_off==0){
    analogWrite(pumppwm,0);          // PWM to turn off Pump
    digitalWrite(IN1,LOW);
    digitalWrite(IN2,LOW);
    Serial.println("WaterPump Off");
   }
    
}
void requestEvent() {

  if(i2cdata[0]==7){          // #7 query for encodercount
   Wire.write(encodercount & 0xFF);  // respond with message encodercount
   Wire.write(encodercount >> 8);
   Wire.write(servo1.read());
   //Serial.println("Sending...");
   i2cdata[0]=0;
  }
 
}


// function that executes whenever data is received from master
//  slave device receives a transmission from a master.
int receiveEvent(int howMany) {
  int i;
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
     //    Serial.print("WriteReg=[");
     //    Serial.print(i2cdata[0]);
     //    Serial.print("] :Num:");
     //    Serial.println(howMany);         
           if(i2cdata[0]== 4 ){
            i2c_robotarm_enable = 1;
            }
      //     else if(i2cdata[0]==5)
     //      {
     //       servo1.write(i2cdata[1]);
     //       Serial.println("servo");
     //      }
    }
 
}
