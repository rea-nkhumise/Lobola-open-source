//Include Libraries
#include <Servo.h>
#include <AccelStepper.h>
#include <MultiStepper.h>

/* Activation Buttons & Potentiometer */
const int btnLid = A0;  // Up-Down Lid Motion [Stepper]  
const int btnFeed = A1; // In-Out Feed Motion [Servo]
const int btnScrew = A2; // Screw Motion [Stepper]
const int btnStove = A3;  // On-Off Stove Motion [Servo]
const int potPin = A4; // Potentiometer [DC Mixer Motor Speed Control]

/* DC Motor Declaration */
const int dcMotorPin = 6; // PWM ~DC motor

/* Stepper Motor Declaration */
const int stepMotorPulse_1 = 5; // Stepper M-1 PWM Stepper
const int stepMotorDir_1 = 4; // Stepper M-1 direction
const int stepMotorPulse_2 = 3; // Stepper M-2 PWM Stepper
const int stepMotorDir_2 = 2; // Stepper M-2 direction
const long rev = 200; // step => 1-full revolution
AccelStepper lidMotor(1,stepMotorPulse_1,stepMotorDir_1); // declare Stepper object
AccelStepper screwMotor(1,stepMotorPulse_2,stepMotorDir_2); // declare Stepper object

/* Servo Motor Declaration */
Servo feedMotor;  // declare Servo object
Servo stoveMotor; // declare Servo object
const int feedMotorPin = 9; // PWM Servo 
const int stoveMotorPin = 10; // PWM Servo

/* Calculation Variables */
int counter[] = {0,0,0,0}; // 4 buttons
int lastButtonState[] = {0,0,0,0}; // 4 button States
int motorSpeed = 0; //variable holfing potPin values

void setup() {
  Serial.begin(9600);
  /* Servo Motor */
  feedMotor.attach(feedMotorPin); // Assigning Pin Feed_Servo
  stoveMotor.attach(stoveMotorPin); // Assigning Pin Stove_Servo
  feedMotor.write(20); // reset to Zero
  delay(500); // reduce jerk: acceleration spikes
  stoveMotor.write(160); // reset to Zero
  delay(500); //reduce jerk: acceleration spikes
  
  /* Stepper Motor */
  screwMotor.setMaxSpeed(500); // 1000 steps per second
  screwMotor.setAcceleration(10000); // 500 steps per second per second
  lidMotor.setMaxSpeed(1000); // 800 steps per second
  lidMotor.setAcceleration(10000); // 500 steps per second per second
 
  /* DC Motor */
  pinMode(dcMotorPin,OUTPUT);
 
  /* Intializing Buttons & Potentiometer */
  pinMode(btnLid,INPUT);
  pinMode(btnFeed,INPUT);
  pinMode(btnStove,INPUT);
  pinMode(btnScrew,INPUT);
  pinMode(potPin,INPUT);
}

void loop() {
  /* Buttons */
  int buttonState[4] = { // array of 5 buttons
    digitalRead(btnLid), // Up-Down Lid Motion [Stepper] 
    digitalRead(btnFeed), // In-Out Feed Motion [Servo]
    digitalRead(btnScrew), // Screw Motion [Stepper]
    digitalRead(btnStove), // On-Off Stove Motion [Servo]
    }; 

  /* Potentiometer Readings */
    motorSpeed = analogRead(potPin); //IN-signals [0 - 1023] units
    motorSpeed = map(motorSpeed, 0, 1023, 0 , 255); //maps [0 - 1023] -> [0 - 255] units

    lidButtonState(buttonState[0], counter[0], lastButtonState[0]); // LID_MOTOR check button state & react  
    feedButtonState(buttonState[1], counter[1], lastButtonState[1]); // FEED_MOTOR check button state & react  
    screwButtonState(buttonState[2], counter[2], lastButtonState[2]); // SCREW_MOTOR check button state & react 
    stoveButtonState(buttonState[3], counter[3], lastButtonState[3]); // STOVE_MOTOR check button state & react 
    mixerButtonState(motorSpeed); // MIXER_MOTOR check button state & react
}


/**********************************************************************
***********************************************************************
Operational Functions
************************************************************************
************************************************************************/
/* LID MOTOR [STEPPER] CONTROL FUNCTIONS */
void lidMotionUp(){
      long pos = -10*rev; // 10* CW revolutions        
      Serial.println("Going UP!");
      lidMotor.runToNewPosition(pos); // -ve pos => upwards  
      delay(500); //necessary to avoid bouncing
  }
void lidMotionDown(){
      long pos = -10*rev; // 10* CW revolutions     
      Serial.println("Going DOWN!");
      lidMotor.runToNewPosition(-pos); // +ve pos => upwards 
      delay(500); //necessary to avoid bouncing
  }
/* FEED MOTOR [SERVO] CONTROL FUNCTIONS */
void feedINpos(){
  Serial.println("INSIDE");
  feedMotor.write(180);
  delay(500);
} 
void feedRESTpos(){
  Serial.println("REST");
  feedMotor.write(20);
  delay(500);
}
void feedSHAKEpos(){
  Serial.println("SHAKE");
  for(byte i = 0; i < 100; i++){
  Serial.print("i = ");
  Serial.println(i);
  feedMotor.write(220);
  delay(100);
  feedMotor.write(160);
  delay(100);}
}
/* STOVE MOTOR [SERVO] CONTROL FUNCTIONS */
void stoveHIGH(){
  Serial.println("HIGH");
  stoveMotor.write(20);
  delay(500);
}
void stoveMEDIUM(){
  Serial.println("MEDIUM");
  stoveMotor.write(90);
  delay(500);
}
void stoveOFF(){
  Serial.println("OFF");
  stoveMotor.write(160);
  delay(500);
}
/* SCREW MOTOR [STEPPER] CONTROL FUNCTIONS */
void screwPour(){
  long rotation = -20*rev; // 5* CW revolutions [40x => 250ml | 20x => 125ml]
  static long k; // allow repeated rotation per button press 
   long val = rotation*(k+1);      
  Serial.println("Pour-In");
      screwMotor.runToNewPosition(rotation*(k+1)); // +ve pos => upwards 
      delay(500);  //necessary to avoid bouncing
      k++; // increment
  }
/*************************************************************************
****************** LID MOTOR BUTTON OPERATION::START**********************
**************************************************************************/
void lidButtonState(int buttonState, int counter_in, int lastButtonState){
      static int counter = counter_in;
      static int initial = counter_in;
      // compare buttonState to its previous state
      if (buttonState != lastButtonState){
       //if the state has change, increment counter
       if(buttonState == HIGH){
          counter++;
          initial++;     
       if(counter == 1){ 
            Serial.println("counter = 1 for LID");
            lidMotionUp();
            } else if(counter == 2){ 
            Serial.println("counter = 2 for LID");
            lidMotionDown();
            }
          }
      // Delay to avoid bouncing
      delay(50); 
      if( counter >= 2){
        counter = 0; // reset counter
        } 
      }
      lastButtonState = buttonState;}
/*************************************************************************
****************** FFED MOTOR BUTTON OPERATION::START**********************
**************************************************************************/
void feedButtonState(int buttonState, int counter_in, int lastButtonState){
      static int counter = counter_in;
      // compare buttonState to its previous state
      if (buttonState != lastButtonState){
       //if the state has change, increment counter
       if(buttonState == HIGH){
          counter++;     
       if(counter == 1){ 
            Serial.println("counter = 1 for FEED-IN");
            feedINpos();
            } else if(counter == 2){ 
            Serial.println("counter = 2 for FEED-SHAKE");
            feedSHAKEpos();
            } else if(counter == 3){ 
            Serial.println("counter = 2 for FEED-REST");
            feedRESTpos();
            }
          }
      // Delay to avoid bouncing
      delay(50); 
      if( counter >= 3){
        counter = 0; // reset counter
        } 
      }
      lastButtonState = buttonState;}
/*************************************************************************
****************** STOVE MOTOR BUTTON OPERATION::START**********************
**************************************************************************/
void stoveButtonState(int buttonState, int counter_in, int lastButtonState){
      static int counter = counter_in;
      // compare buttonState to its previous state
      if (buttonState != lastButtonState){
       //if the state has change, increment counter
       if(buttonState == HIGH){
          counter++;     
       if(counter == 1){ 
            Serial.println("counter = 1 for STOVE-HIGH");
            stoveHIGH();
            } else if(counter == 2){ 
            Serial.println("counter = 2 for STOVE-MEDIUM");
            stoveMEDIUM();
            } else if (counter == 3){
            Serial.println("counter = 3 for STOVE-OFF");
            stoveOFF();
            }
          }
      // Delay to avoid bouncing
      delay(50); 
      if( counter >= 3){
        counter = 0; // reset counter
        } 
      }
      lastButtonState = buttonState;}
/*************************************************************************
****************** SCREW MOTOR BUTTON OPERATION::START**********************
**************************************************************************/
void screwButtonState(int buttonState, int counter_in, int lastButtonState){
      static int counter = counter_in;
      // compare buttonState to its previous state
      if (buttonState != lastButtonState){
       //if the state has change, increment counter
       if(buttonState == HIGH){
          counter++;     
       if(counter == 1){ 
            Serial.println("counter = 1 for SCREW-POUR");
            screwPour();
            }
          }
      // Delay to avoid bouncing
      delay(50); 
      if( counter >= 1){
        counter = 0; // reset counter
        } 
      }
      lastButtonState = buttonState;}
/*************************************************************************
****************** MIXER MOTOR BUTTON OPERATION::START**********************
**************************************************************************/
void mixerButtonState(int motorSpeed){
    Serial.print("Motor Speed = ");
    Serial.println(motorSpeed);
    analogWrite(dcMotorPin,motorSpeed);
     if (motorSpeed > 0){
      long duration = millis(); // start keeping time
      Serial.print("Duration = ");
      Serial.println(duration/1000);
      }
}

/************************************************************************************************

*     Company: Planet Eye Pty Ltd
*     Project: Lobola - robotic device for cooking pap (sadza)
*     Designer: Rea Nkhumise
*     Year: 2020
*     Country: South Africa

*     MIT License

*     Copyright (c) 2020 Planet Eye Pty Ltd.

*     Permission is hereby granted, free of charge, to any person obtaining a copy
*     of this software and associated documentation files (the "Software"), to deal
*     in the Software without restriction, including without limitation the rights
*     to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
*     copies of the Software, and to permit persons to whom the Software is
*     furnished to do so, subject to the following conditions:

*     The above copyright notice and this permission notice shall be included in all
*     copies or substantial portions of the Software.

*     THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
*     IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
*     FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
*     AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
*     LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
*     OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
*     SOFTWARE.

***************************************************************************************************/