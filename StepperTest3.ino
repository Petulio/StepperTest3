#define BLYNK_PRINT Serial
#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <Stepper.h>
#include <SoftwareSerial.h>

char auth[] = "dfd7a1badb3c4c3094b644253ac42289"; // AUTH TOKEN FROM BLYNK

// WiFi credentials.
char ssid[] = "OnePlusPlus";
char pass[] = "heyhey10";

const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution on the motor

SoftwareSerial EspSerial(2, 4); // RX, TX
Stepper myStepper(stepsPerRevolution, 12,13);

// give the motor control pins names:
const int pwmA = 3;
const int pwmB = 11;
const int brakeA = 9;
const int brakeB = 8;
const int dirA = 12;
const int dirB = 13;

// give the limit switch a pin name:
const int limitSwitch = 6;

// The ESP8266 baud rate:
#define ESP8266_BAUD 9600

int inVal = 10;
int steps = 0;
int stepSpeed;
int limitVal = 0;

void setup()
{
  Serial.begin(9600);

  // Set ESP8266 baud rate
  EspSerial.begin(ESP8266_BAUD);
  delay(10);

  ESP8266 wifi(&EspSerial);

  Blynk.virtualWrite(V1, 0); // Set value of virtual pin
  Blynk.begin(auth, wifi, ssid, pass); // Start connection

  pinMode(pwmA, OUTPUT);
  pinMode(pwmB, OUTPUT);
  pinMode(brakeA, OUTPUT);
  pinMode(brakeB, OUTPUT);
  pinMode(limitSwitch, INPUT_PULLUP);
  
  digitalWrite(pwmA, HIGH);
  digitalWrite(pwmB, HIGH);
  digitalWrite(brakeA, LOW);
  digitalWrite(brakeB, LOW);
  
  myStepper.setSpeed(30); // Set speed for homing

  Blynk.virtualWrite(V4, "Homing..."); // Send string to display on Blynk

  // Start homing procedure:
  do{
    limitVal = digitalRead(limitSwitch);
    myStepper.step(-1);
    }while(limitVal == LOW);
    
  // Move blind forward untill switch is released:
  do{
    limitVal = digitalRead(limitSwitch);
    myStepper.step(1);
    }while(limitVal == HIGH);

  Blynk.virtualWrite(V4, "Now home!");
  delay(1000);
  Blynk.virtualWrite(V4, "Ready to use:");

}

void loop()
{
  Blynk.run(); // Function to run the blynk program

  // if the stepper is between 0 or 1000 (step position) then you are able to move it
  if(steps >= 0 && steps <= 1000){
    if(inVal < 10){
      myStepper.step(-1);
      steps--;
      Serial.println(steps);
      }else if(inVal > 10){
        myStepper.step(1);
        steps++;
        Serial.println(steps);
        }
  }
  // if the stepper moves beyond the limit, then make it step back to the correct limit spot (0 or 1000)
  while(steps == -1){
    myStepper.step(1);
    steps++;
    Blynk.virtualWrite(V4, "Blind at limit!");
    delay(2000);
    Blynk.virtualWrite(V4, "Ready to use:");
    break;
    }
  while(steps == 1001){
    myStepper.step(-1);
    steps--;
    Blynk.virtualWrite(V4, "Blind at limit!");
    delay(2000);
    Blynk.virtualWrite(V4, "Ready to use:");
    break;
    }
}

// the virtual pins created in the Blynk app that sends an integer if a button is pushed:
BLYNK_WRITE(V1){
  if(param.asInt() == 1){
    inVal = 5;
    }else{
      inVal = 10;
      }
  }
BLYNK_WRITE(V2){
  if(param.asInt() == 1){
    inVal = 15;
    }else{
      inVal = 10;
      }
  }
BLYNK_WRITE(V3){
  stepSpeed = param.asInt();
  myStepper.setSpeed(stepSpeed);
  }
