
#include <MozziGuts.h>
#include <Oscil.h>
#include <tables/sin2048_int8.h>
#include <math.h>

Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> sineOsc(SIN2048_DATA);
// at the top of your sketch
#define CONTROL_RATE 64   // or some other power of 2

int sound = 0;

//Capteur ultrason
const int echoPin = 31; // the SRF05's echo pin
const int initPin = 33; // the SRF05's init pin
int pulseTime = 0; // variable for reading the pulse

const int pinLedBoot = 13;


int freq = 262;

int idx = 0;
int seuil[23]     = {269, 285, 302, 320, 339, 359, 381, 404, 428, 453, 480, 509, 539, 571, 605, 641, 679, 719, 762, 807, 855, 906, 960};
int seuillage[24] = {262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988};

int last5[5] = [262,262,262,262,262]; 
int med[5]   = [262,262,262,262,262];
  
static const byte sine_tab[256] =
{
        128, 131, 134, 137, 140, 143, 146, 149,
        152, 156, 159, 162, 165, 168, 171, 174,
        176, 179, 182, 185, 188, 191, 193, 196,
        199, 201, 204, 206, 209, 211, 213, 216,
        218, 220, 222, 224, 226, 228, 230, 232,
        234, 236, 237, 239, 240, 242, 243, 245,
        246, 247, 248, 249, 250, 251, 252, 252,
        253, 254, 254, 255, 255, 255, 255, 255,
        255, 255, 255, 255, 255, 255, 254, 254,
        253, 252, 252, 251, 250, 249, 248, 247,
        246, 245, 243, 242, 240, 239, 237, 236,
        234, 232, 230, 228, 226, 224, 222, 220,
        218, 216, 213, 211, 209, 206, 204, 201,
        199, 196, 193, 191, 188, 185, 182, 179,
        176, 174, 171, 168, 165, 162, 159, 156,
        152, 149, 146, 143, 140, 137, 134, 131,
        128, 124, 121, 118, 115, 112, 109, 106,
        103,  99,  96,  93,  90,  87,  84,  81,
         79,  76,  73,  70,  67,  64,  62,  59,
         56,  54,  51,  49,  46,  44,  42,  39,
         37,  35,  33,  31,  29,  27,  25,  23,
         21,  19,  18,  16,  15,  13,  12,  10,
          9,   8,   7,   6,   5,   4,   3,   3,
          2,   1,   1,   0,   0,   0,   0,   0,
          0,   0,   0,   0,   0,   0,   1,   1,
          2,   3,   3,   4,   5,   6,   7,   8,
          9,  10,  12,  13,  15,  16,  18,  19,
         21,  23,  25,  27,  29,  31,  33,  35,
         37,  39,  42,  44,  46,  49,  51,  54,
         56,  59,  62,  64,  67,  70,  73,  76,
         79,  81,  84,  87,  90,  93,  96,  99,
        103, 106, 109, 112, 115, 118, 121, 124
};

void setup() {
  startMozzi(CONTROL_RATE);
  sineOsc.setFreq(freq);

  
  pinMode(pinLedBoot, OUTPUT);
  // make the echo pin an input:
  pinMode(initPin, OUTPUT);
  pinMode(echoPin, INPUT);
  digitalWrite(pinLedBoot, 1);

}

void updateControl() {
  // your control code
  digitalWrite(initPin, LOW);
  delayMicroseconds(2500);
  digitalWrite(initPin, HIGH); // send signal
  delayMicroseconds(20); // wait 50 milliseconds for it to return
  digitalWrite(initPin, LOW); // close signal
  
  //100micro à 25milli
  pulseTime = pulseIn(echoPin, HIGH); // calculate time for signal to return

  freq = map(pulseTime, 100, 25000, 261, 988);
}

int updateAudio() {
  // your audio code which returns an int between -244 and 243
  // actually, a char is fine

  /*freq=(freq+1)%1000;
  if(freq>seuil[idx])
  {
    idx=(idx+1)%23;
    sineOsc.setFreq(seuillage[idx]);
  }
  */

  for (int i = 0; i < 23; i++){
    if(freq < seuil[i]){
      freq = seuillage[i];
      break;
    }    
  }
  if(freq > seuil[22]){
      freq = seuillage[23];
  }

  int tmp = last5[0];
  last5[0]=freq;
  last5[4]=last5[3];
  last5[3]=last5[2];
  last5[2]=last5[1];
  last5[1]=tmp;

  // trier et renvoyer medianne.

  sineOsc.setFreq(freq);
  
  return sineOsc.next();
}

void loop() {
  audioHook(); // fills the audio buffer
}

