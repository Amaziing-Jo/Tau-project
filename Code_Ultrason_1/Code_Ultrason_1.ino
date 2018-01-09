
#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/sin2048_int8.h>
#include <tables/saw2048_int8.h>
#include <tables/triangle_valve_2048_int8.h>
#include <math.h>

Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> sineOsc1(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> sineOsc2(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> sineOsc3(SIN2048_DATA);
Oscil <SAW2048_NUM_CELLS, AUDIO_RATE> sawOsc(SAW2048_DATA);
Oscil <TRIANGLE_VALVE_2048_NUM_CELLS, AUDIO_RATE> triOsc(TRIANGLE_VALVE_2048_DATA);

// at the top of your sketch
#define CONTROL_RATE 64   // or some other power of 2

//Capteur ultrason
const int echoPin = 31; // the SRF05's echo pin
const int initPin = 33; // the SRF05's init pin

const int echoPinVolume = 35; // the SRF05's echo pin
const int initPinVolume = 37; // the SRF05's init pin

const int switch1Pin = 22;
const int switch2Pin = 23;

int pulseTimeFreq = 0; // variable for reading the pulse
int pulseTimeVolume = 0; // variable for reading the pulse for volume

int lastPulseTimeFreq = 0;
int gamme = 4;
int numberOfGamme = 1;
int multip = 2;

int mode = 0;

unsigned int minValue = 9999;
unsigned int maxValue = 0;
int count = 0;
int pinLedBoot = 13;
long timer;

int freq = 440;
int volume = 0;
int lastFreq = 262;

int idx = 0;


void setup() {
  //Serial.begin(9600);
 
  pinMode(pinLedBoot, OUTPUT);
  // make the echo pin an input:
  pinMode(initPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(initPinVolume, OUTPUT);
  pinMode(echoPinVolume, INPUT);

  pinMode(switch1Pin, INPUT);
  pinMode(switch2Pin, INPUT); 

  minValue = 100;
  maxValue = 3000;

  startMozzi(CONTROL_RATE);
  
}

void updateControl() {
  // your control code
  numberOfGamme = digitalRead(switch1Pin) + 1; // +1
  gamme = digitalRead(switch2Pin) + 3; //+ 3;

  //Serial.print("numberOfGamme : "); Serial.println(numberOfGamme);
  //Serial.print("gamme : "); Serial.println(gamme);
  
  if(count != 0)
  {
      digitalWrite(initPin, LOW);
      delayMicroseconds(15); //Wait to be sure the signals is interprated
      digitalWrite(initPin, HIGH); // send signal
      delayMicroseconds(15); // trigger pulse 
      digitalWrite(initPin, LOW); // close signal
      //100micro à 30milli
      pulseTimeFreq = pulseIn(echoPin, HIGH); // calculate time for signal to return
  }
  else
  {
      digitalWrite(initPinVolume, LOW);
      delayMicroseconds(15); //Wait to be sure the signals is interprated
      digitalWrite(initPinVolume, HIGH); // send signal
      delayMicroseconds(15); // trigger pulse 
      digitalWrite(initPinVolume, LOW); // close signal
      //100micro à 30milli
      pulseTimeVolume = pulseIn(echoPinVolume, HIGH); // calculate time for signal to return
  }

  if(pulseTimeVolume > maxValue)
  {
    pulseTimeVolume = 255;
  }  
  if(pulseTimeFreq > maxValue)
  {
    pulseTimeFreq = lastPulseTimeFreq;
  }
  if(pulseTimeFreq < minValue) pulseTimeFreq=minValue;
  if(pulseTimeVolume < minValue) pulseTimeVolume=minValue;

  lastPulseTimeFreq = pulseTimeFreq;

  multip = 1;
  for(int i=0; i<gamme; i++)
  {
    multip*=2 ;
  }
  
  freq = map(pulseTimeFreq, minValue, maxValue, 65 * multip, 124 * multip * numberOfGamme);
  volume = map(pulseTimeVolume, minValue, maxValue, 0, 255);
  count++;
  count = count % 10;
  
}

int updateAudio() {

  int result;
    switch (mode)
    {
      case 0 :
        sineOsc1.setFreq(freq);
        result = (int)sineOsc1.next();
        break;
      case 1 :
        sineOsc1.setFreq(freq);
        sineOsc2.setFreq(freq*4);
        sineOsc3.setFreq(freq*16);
        result = (int)sineOsc1.next() + (int)sineOsc2.next()/2 + (int)sineOsc3.next()/4;
        break;
      case 2:
        sineOsc1.setFreq(freq);
        sineOsc2.setFreq(freq*2);
        sineOsc3.setFreq(freq*8);
        result = (int)sineOsc1.next() + (int)sineOsc2.next()/2 + (int)sineOsc3.next()/4;
        break;
      case 3 :
        sawOsc.setFreq(freq);
        result = (int)sawOsc.next();
        break;
      case 4 :
        triOsc.setFreq(freq);
        result = (int)triOsc.next();
        break;
      default:
        sineOsc1.setFreq(freq);
        result = (int)sineOsc1.next();
        break;
    }
    
    
    return (result * volume) >> 8;
  
}

void loop() {
  audioHook(); // fills the audio buffer
}

