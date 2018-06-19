/*
  FREE TO USE
  Code by Abishek Venkit
  
  arduinoFFT sampling program taken from:
  Copyright (C) 2018 Enrique Condés and Ragnar Ranøyen Homb
*/

#include "FastLED.h" //for controlling Neopixel LED strip
#include "arduinoFFT.h" //for fft procecssing of audio in
#define NUM_LEDS 300 //number of total leds in strip
#define DATA_PIN 7 //DATA out pin from arduino to LED strip
#define CHANNEL A0 //Analog input

CRGB leds[NUM_LEDS]; //Create LED array
arduinoFFT FFT = arduinoFFT(); //Create FFT object 

const int samples = 32; //This value MUST ALWAYS be a power of 2
const double samplingFrequency = 1000; //Hz, must be less than 10000 due to ADC
unsigned int sampling_period_us; 
unsigned long microseconds;
uint8_t bufferSize = samples >> 1; //bufferSize gives total number of frequency bins
int fmapmin = ((1.0*samplingFrequency)/samples); //calculate frequency of the lowest freqbin 
int fmapmax = (((bufferSize-1)*samplingFrequency)/samples); //calculate frequency of the highest freqbin

int level; //initialize level variable to keep track of amplitude
int prevLevel; //initialize prevLevel variable to keep track of previous amplitude
uint8_t prevLevels[25] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}; //initialize prevLevels array
int ledmap[10] = {1,55,61,115,121,175,181,235,241,295}; 
//ledmap array maps the string of neopixel leds to a 25 by 10 grid by using an offset based on freqbin

int micPin = 6;

void setup() {
  sampling_period_us = round(1000000*(1.0/samplingFrequency)); //microseconds
  pinMode(micPin, INPUT);        // sets the digital pin 7 as input
  //Serial.begin(9600);
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS); //add correct LED strip
  FastLED.setBrightness(16); //brightness value above 20 becomes too bright in dark settings
}

void loop() {
  //Sampling
  double vReal[samples]; //arrays of real and imaginary parts of sampled values
  double vImag[samples];
  for(int i=0; i<samples; i++) {
      microseconds = micros(); //Overflows after around 70 minutes!
      vReal[i] = analogRead(CHANNEL);
      vImag[i] = 0;
      while(micros() < (microseconds + sampling_period_us)) {
        //empty loop
      }
  }
  
  //Compute FFT data
  FFT.Windowing(vReal, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD); //Weigh data
  FFT.Compute(vReal, vImag, samples, FFT_FORWARD); //Compute FFT
  FFT.ComplexToMagnitude(vReal, vImag, samples); //Compute magnitudes
  
  for (int i = 1; i < bufferSize; i++) { //iterate for each freqbin
    double freq = ((i * 1.0 * samplingFrequency) / samples); //calculate current freq
    //Serial.print("freq: ");
    //Serial.println(freq);
    double amp = (vReal[i]); //calculate amplitude of current freq
    //Serial.print("amp: ");
    //Serial.println(amp);
    
    //for microphone noise reduction
    if (digitalRead(micPin) == HIGH) {
      if (freq == 31.25) {
        amp = amp - 3900;
      }
      if (freq == 62.5) {
        amp = amp - 90;
      }
    }
    //for microphone noise reduction
    
    int level = map(amp, 0, 200, 0, 10); //map amp value to a level 0 to 10
    level = constrain(level,0,10);
    int freqbin = map(freq, fmapmin, fmapmax, 0, 24); //map freq value to a bin 0 to 24
    //Serial.print("freqbin: ");
    //Serial.println(freqbin);
    freqbin = constrain(freqbin,0,24);
    int offset = freqbin; //offset for LED grid is the same as freqbin
    //Serial.print("offset: ");
    //Serial.println(offset);
    int prevLevel = prevLevels[freqbin]; //prevLevel variable is taken from prevLevels array
    //Serial.print("prevLevel: ");
    //Serial.println(prevLevel);

    if (level > prevLevel) { //condition to light up more leds in a specific column
      for (int i=prevLevel; i<level; i++) {       
        if (i==0||i==2||i==4||i==6||i==8) { //offset conditions
          //leds[ledmap[i]+offset] = CHSV((171-(17.1*level)), 255, 200);
          if (i<3) {
              leds[ledmap[i]+offset] = CRGB::Cyan; //bottom 3 rows
          }
          else if (i>2 && i<7) {
              leds[ledmap[i]+offset] = CRGB::Purple; //middle 4 rows
          }
          else if (i>6) {
            leds[ledmap[i]+offset] = CRGB::Red; //top 3 rows
          }
        }
        else if (i==1||i==3||i==5||i==7||i==9) { //offset conditions
           //leds[ledmap[i]-offset] = CHSV((171-(17.1*level)), 255, 200);
          if (i<3) {
              leds[ledmap[i]-offset] = CRGB::Cyan; //bottom 3 rows
          }
          else if (i>2 && i<7) {
              leds[ledmap[i]-offset] = CRGB::Purple; //middle 4 rows
          }
          else if (i>6) {
              leds[ledmap[i]-offset] = CRGB::Red; //top 3 rows
          }
        }
      }
    }
    if (level < prevLevel) { //condition to deactivate LEDs (when the level is decreasing)
      for (int i=(prevLevel)-1; i>=level; i--) {
        if (i==0||i==2||i==4||i==6||i==8) { //offset condition
            leds[ledmap[i]+offset] = CRGB::Black;
        }
        else if (i==1||i==3||i==5||i==7||i==9) { //offset condition
            leds[ledmap[i]-offset] = CRGB::Black;
        }          
      }
    }  
    prevLevels[freqbin] = level; //update the prevLevels array
  }
  FastLED.show(); //update LED display
}
    

