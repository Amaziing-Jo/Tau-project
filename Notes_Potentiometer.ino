/*  Example playing a sinewave at a set frequency,
    using Mozzi sonification library.
  
    Demonstrates the use of Oscil to play a wavetable.
  
    Circuit: Audio output on digital pin 9 on a Uno or similar, or
    DAC/A14 on Teensy 3.1, or 
    check the README or http://sensorium.github.com/Mozzi/
  
    Mozzi help/discussion/announcements:
    https://groups.google.com/forum/#!forum/mozzi-users
  
    Tim Barrass 2012, CC by-nc-sa.
*/

#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

//Pin input potentiometer
const char INPUT_PIN = 0; // set the input for the knob to analog pin 0

//Capteur ultrason
const int echoPin = 31; // the SRF05's echo pin
const int initPin = 33; // the SRF05's init pin
int pulseTime = 0; // variable for reading the pulse

//Capteur ultrason volume
const int echoPinVol = 35; // the SRF05's echo pin
const int initPinVol = 37; // the SRF05's init pin
int pulseTimeVol = 0; // variable for reading the pulse

unsigned int minValue = 99999;
unsigned int maxValue = 0;
unsigned int minValueVol = 99999;
unsigned int maxValueVol = 0;
int pinLedBoot = 13;
long timer;

//frequence joué selon potentiomètre
int frequence;
int seuil[23] = {269, 285, 302, 320, 339, 359, 381, 404, 428, 453, 480, 509, 539, 571, 605, 641, 679, 719, 762, 807, 855, 906, 960};
int seuillage[24] = {262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988};

//Réglage volume
byte volume;

// use #define for CONTROL_RATE, not a constant
#define CONTROL_RATE 64 // powers of 2 please


void setup(){
 
  pinMode(pinLedBoot, OUTPUT);
  // make the echo pin an input:
  pinMode(initPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(pinLedBoot, 1);

  timer = millis();

  while(millis() - timer < 5000){
    digitalWrite(initPin, LOW);
    delayMicroseconds(25000);
    digitalWrite(initPin, HIGH); // send signal
    delayMicroseconds(20); // wait 50 milliseconds for it to return
    digitalWrite(initPin, LOW); // close signal
    pulseTime = pulseIn(echoPin, HIGH); // calculate time for signal to return
    
    if(pulseTime < minValue && pulseTime > 0){
      minValue = pulseTime;
    }
     if(pulseTime > maxValue && pulseTime > 0){
      maxValue = pulseTime;
    }    

    digitalWrite(initPinVol, LOW);
    delayMicroseconds(25000);
    digitalWrite(initPinVol, HIGH); // send signal
    delayMicroseconds(20); // wait 50 milliseconds for it to return
    digitalWrite(initPinVol, LOW); // close signal
    pulseTimeVol = pulseIn(echoPinVol, HIGH); // calculate time for signal to return
    
    if(pulseTimeVol < minValueVol){
      minValueVol = pulseTimeVol;
    }
     if(pulseTimeVol > maxValueVol){
      maxValueVol = pulseTimeVol;
    }    
  }
  digitalWrite(pinLedBoot, 0);
  /*
  Serial.println("Initialisation terminée");
  Serial.print("minValue : "); Serial.println(minValue);
  Serial.print("maxValue : "); Serial.println(maxValue);
  Serial.print("minValueVol : "); Serial.println(minValueVol);
  Serial.print("maxValueVol : "); Serial.println(maxValueVol);
  */
  startMozzi(CONTROL_RATE); // set a control rate of 64 (powers of 2 please)
}


void updateControl(){

  
  digitalWrite(initPin, LOW);
  delayMicroseconds(25000);
  digitalWrite(initPin, HIGH); // send signal
  delayMicroseconds(20); // wait 50 milliseconds for it to return
  digitalWrite(initPin, LOW); // close signal
  pulseTime = pulseIn(echoPin, HIGH); // calculate time for signal to return
  //int distance = (pulseTime/58) * 150; // convert to centimeter
  frequence = map(pulseTime, minValue, maxValue, 261, 988);

  digitalWrite(initPinVol, LOW);
  delayMicroseconds(25000);
  digitalWrite(initPinVol, HIGH); // send signal
  delayMicroseconds(20); // wait 50 milliseconds for it to return
  digitalWrite(initPinVol, LOW); // close signal
  pulseTimeVol = pulseIn(echoPinVol, HIGH); // calculate time for signal to return
  //int distance = (pulseTime/58) * 150; // convert to centimeter
  volume = map(pulseTimeVol, minValueVol, maxValueVol, 0, 255);

  /*
   // read the variable resistor for volume
  int sensor_value = mozziAnalogRead(INPUT_PIN); // value is 0-1023
  // map it to an 8 bit range for efficient calculations in updateAudio
  frequence = map(sensor_value, 0, 1023, 261, 988);
  */
  
  

  for (int i = 0; i < 23; i++){
    if(frequence < seuil[i]){
      frequence = seuillage[i];
      break;
    }    
  }
  if(frequence > seuil[22]){
      frequence = seuillage[23];
  }
}


int updateAudio(){
  aSin.setFreq(frequence);
  //return aSin.next(); // return an int signal centred around 0
  //Volume
  return ((int)aSin.next() * volume)>>8;
}


void loop(){
  audioHook(); // required here
}



