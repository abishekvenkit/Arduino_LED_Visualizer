# Arduino_LED_Visualizer
Takes in an audio signal and converts it to a frequency representation

This program uses the library "arduinoFFT" to perform a fast fourier transform on an incoming audio signal. For the specific harware implementation used in this project, A Neopixel (WS2812B) LED strip was used. Other LED strip can be used with the code. For the visualizer that was made, there needed to be mapping from the strip array to a grid array, because the final product was an array of 25x10 LEDs (the strip was held in a snaked pattern to create this array). The portions with "offsets" and the array ledmap are solely for this mapping proccess.

The code allows for either direct audio jack input or microphone input (for which an non-inverting amplifier circuit was built). There is a noise reduction component for the microphone that is written for the specific hardware implementation used and would need to be modified for different implementations.
