
#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/sin2048_int8.h>
#include <tables/saw2048_int8.h>
#include <tables/triangle_valve_2048_int8.h>
#include <math.h>

Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> sineOsc1(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> sineOsc2(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> sineOsc3(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> sineOsc4(SIN2048_DATA);
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> sineOsc5(SIN2048_DATA);
Oscil <SAW2048_NUM_CELLS, AUDIO_RATE> sawOsc(SAW2048_DATA);
Oscil <TRIANGLE_VALVE_2048_NUM_CELLS, AUDIO_RATE> triOsc(TRIANGLE_VALVE_2048_DATA);

// at the top of your sketch
#define CONTROL_RATE 64   // or some other power of 2

// haut-parleur pin 11

//Capteur ultrason
const int echoPin = 31; // the SRF05's echo pin
const int initPin = 33; // the SRF05's init pin

const int echoPinVolume = 35; // the SRF05's echo pin
const int initPinVolume = 37; // the SRF05's init pin

const int mode1Pin = 49;
const int mode2Pin = 48;
const int mode3Pin = 47;
const int mode4Pin = 46;
const int mode5Pin = 45;
const int mode6Pin = 44;

const int switch1Pin = 22;
const int switch2Pin = 23;

int pulseTimeFreq = 0; // variable for reading the pulse
int pulseTimeVolume = 0; // variable for reading the pulse for volume

float K  = 0;
float D0 = 0;

int lastPulseTimeFreq = 0;
int gamme = 4;
int lastGamme = 0;
int numberOfGamme = 1;
unsigned int multip = 2;

int mode = 0;

float fmin_ = 261.63 ;
float fmax_ = 1046.5 ;

unsigned int minValue = 9999;
unsigned int maxValue = 0;
unsigned int maxValueVol = 0;
int count = 0;
int pinLedBoot = 13;
long timer;

int freq = 440;
int volume = 0;
int lastFreq = 262;

int idx = 0;


float computeK()
{
  float k = 0;

  k = fmin_ * fmax_;

  k = k * (maxValue - minValue);

  k = k / (fmax_ - fmin_) ;
  
  return k ;
}

float computeD0()
{
  float d0 = 0;

  d0 = fmin_ * maxValue;

  d0 = d0 - (fmax_ * minValue) ;

  d0 = d0 / (fmax_ - fmin_) ;
  
  return d0;
}

void setup() {
  //Serial.begin(9600);
 
  pinMode(pinLedBoot, OUTPUT);
  // make the echo pin an input:
  pinMode(initPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(initPinVolume, OUTPUT);
  pinMode(echoPinVolume, INPUT);

  pinMode(switch1Pin, INPUT_PULLUP);
  pinMode(switch2Pin, INPUT_PULLUP); 

  pinMode(mode1Pin, INPUT_PULLUP);
  pinMode(mode2Pin, INPUT_PULLUP);
  pinMode(mode3Pin, INPUT_PULLUP);
  pinMode(mode4Pin, INPUT_PULLUP);
  pinMode(mode5Pin, INPUT_PULLUP);
  pinMode(mode6Pin, INPUT_PULLUP);

  minValue = 200;
  maxValue = 2500;
  maxValueVol = 1200;

  K=computeK();
  D0=computeD0();

  startMozzi(CONTROL_RATE);
  
}

void updateControl() {
  // your control code
  //numberOfGamme = digitalRead(switch1Pin) + 1;
  //gamme = digitalRead(switch2Pin) + 3;

  // debug:
  //numberOfGamme = 2;
  //gamme = 3 ;
  gamme = digitalRead(switch1Pin)<<1 + digitalRead(switch2Pin) ;

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

  if(lastGamme != gamme)
  {
    if(gamme == 0)
    {
      fmin_ = 261.63; 
      fmax_ = 1046.5;
    } 
    else if(gamme == 1)
    {
      fmin_ = 261.63;
      fmax_ = 2093;
    }
    else if(gamme == 2)
    {
      fmin_ = 523.25;
      fmax_ =2093;
    }
    else if(gamme == 3)
    {
      fmin_ = 523.25;
      fmax_ = 4186.01;
    }
    
    K = computeK();
    D0 = computeD0();
  }
  
  if(pulseTimeVolume > maxValueVol)
  {
    pulseTimeVolume = maxValueVol;
  }  
  if(pulseTimeFreq > maxValue)
  {
    pulseTimeFreq = lastPulseTimeFreq;
  }
  if(pulseTimeFreq < minValue) pulseTimeFreq=minValue;
  if(pulseTimeVolume < minValue) pulseTimeVolume=minValue;

  lastPulseTimeFreq = pulseTimeFreq;

  lastGamme = gamme;

  

  //freq = 3000 * 65 * multip / (pulseTimeFreq << 3) + 10 ;
  freq = K / (pulseTimeFreq + D0) ;
  //Serial.println(freq);
  //freq = map(pulseTimeFreq, minValue, maxValue, 65 * multip, 124 * multip * numberOfGamme);
  volume = map(pulseTimeVolume, minValue, maxValueVol, 0, 255);
  count++;
  count = count % 10;
  
}

int updateAudio() {

  int result;
  /*{
    if(!digitalRead(mode1Pin)) mode = 0;
    else if(!digitalRead(mode2Pin)) mode = 1;
    else if(!digitalRead(mode3Pin)) mode = 2;
    else if(!digitalRead(mode4Pin)) mode = 3;
    else if(!digitalRead(mode5Pin)) mode = 4;
    else mode = 5;
  }
  */
  // debug:
  mode = 0;

  //Serial.println(freq);
  
  switch (mode)
    {
      case 0 :
        sineOsc1.setFreq(freq);
        result = (int)sineOsc1.next();
        break;
      case 1 :
        sineOsc1.setFreq(freq);
        sineOsc2.setFreq(freq*2);
        sineOsc3.setFreq(freq*4);
        result = (int)( sineOsc1.next()/3.0 + 2.0*sineOsc2.next()/3.0 + sineOsc3.next()/3.0 );
        break;
      case 2:
        sineOsc1.setFreq(freq);
        sineOsc2.setFreq(freq*3);
        sineOsc3.setFreq(freq*5);
        result = (int) ( 5.0*sineOsc1.next()/8.0 + sineOsc2.next()/4.0 + sineOsc3.next()/8.0 );
        break;
      case 3 :
        sawOsc.setFreq(freq);
        result = (int)sawOsc.next();
        break;
      case 4 :
        triOsc.setFreq(freq);
        result = (int)triOsc.next();
        break;
      case 5 :
        /*sineOsc1.setFreq(freq);
        sineOsc2.setFreq(freq*3);
        sineOsc3.setFreq(freq*5);
        sineOsc4.setFreq(freq*7);
        sineOsc5.setFreq(freq*4);
        result = (int)sineOsc1.next()/3 + 3*(int)sineOsc2.next()/10 + (int)sineOsc3.next()/6 + 2*(int)sineOsc4.next()/15 + (int)sineOsc5.next()/15;*/

        sineOsc1.setFreq(freq);
        sineOsc2.setFreq(freq*2);
        sineOsc3.setFreq(freq*3);
        sineOsc4.setFreq(freq*6);
        sineOsc5.setFreq(freq*7);
        result = int( sineOsc1.next()/5.0 + 2*sineOsc2.next()/5.0 + sineOsc3.next()/5.0 + 4*sineOsc4.next()/45.0 + sineOsc5.next()/9.0 );
        break;
      default:
        sineOsc1.setFreq(freq);
        result = (int)sineOsc1.next();
        break;
    }
    //Serial.println(result);
    return (result * volume) >> 8;
}

void loop()
{
  audioHook(); // fills the audio buffer
}

