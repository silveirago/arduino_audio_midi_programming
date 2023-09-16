/*
This code reads the Arduino analog pins and send MIDI CC using the MIDIUSB lib
Tutorial: https://youtu.be/hcm5H6f8MI8

by Gustavo Silveira, a.k.a. Nerd Musician
Learn how to builf your MIDI controllers: https://go.musiconerd.com/
*/

#include "MIDIUSB.h"               // Include the MIDIUSB library for MIDI communication
#include <ResponsiveAnalogRead.h>  // Include the ResponsiveAnalogRead library from https://github.com/dxinteractive/ResponsiveAnalogRead

const int N_POTS = 5;  // Define the number of potentiometers

int potPin[N_POTS] = { A0, A1, A2, A3, A4 };  // Define analog input pins for the potentiometers
int potCC[N_POTS] = { 11, 12 };  // Define MIDI control change (CC) values for the potentiometers

int potReading[N_POTS] = { 0 };  // Initialize an array to store raw potentiometer readings
int potState[N_POTS] = { 0 };    // Initialize an array to store smoothed potentiometer values
int potPState[N_POTS] = { 0 };   // Initialize an array to store the previous potentiometer values

int midiState[N_POTS] = { 0 };    // Initialize an array to store mapped MIDI values
int midiPState[N_POTS] = { 0 };   // Initialize an array to store the previous MIDI values

const byte potThreshold = 15;  // Define a threshold for potentiometer value change
const int POT_TIMEOUT = 300;  // Define a timeout for detecting potentiometer changes
unsigned long pPotTime[N_POTS] = { 0 };  // Initialize an array to store the last time a potentiometer changed
unsigned long potTimer[N_POTS] = { 0 };  // Initialize an array to store the time elapsed since a potentiometer change

byte POT_CH = 0;  // Define the MIDI channel for the potentiometers

float snapMultiplier = 0.01;                      // Define a multiplier for smoothing potentiometer readings
ResponsiveAnalogRead responsivePot[N_POTS] = {};  // Create an array for responsive potentiometers. It gets filled in the Setup.

void setup() {
  // Setup code runs once when the Arduino is powered on or reset.

  Serial.begin(9600);  // Initialize serial communication for debugging

  for (int i = 0; i < N_POTS; i++) {
    responsivePot[i] = ResponsiveAnalogRead(0, true, snapMultiplier);  // Initialize responsive potentiometers
    responsivePot[i].setAnalogResolution(1023);  // Set the analog resolution
  }
}

void loop() {
  // Main loop code runs repeatedly.

  for (int i = 0; i < N_POTS; i++) {

    potReading[i] = analogRead(potPin[i]);  // Read the raw potentiometer value
    responsivePot[i].update(potReading[i]);  // Update the responsive potentiometer
    potState[i] = responsivePot[i].getValue();  // Get the smoothed potentiometer value
    midiState[i] = map(potState[i], 0, 1023, 0, 128);  // Map the potentiometer value to MIDI range

    int potVar = abs(potState[i] - potPState[i]);  // Calculate the absolute change in potentiometer value

    if (potVar > potThreshold) {
      pPotTime[i] = millis();  // Record the time when the potentiometer changed
    }

    potTimer[i] = millis() - pPotTime[i];  // Calculate the time elapsed since the potentiometer changed

    if (potTimer[i] < POT_TIMEOUT) {
      if (midiState[i] != midiPState[i]) {
        
        controlChange(POT_CH, potCC[i], midiState[i]);  // Send a MIDI control change message
        MidiUSB.flush();  // Flush the MIDI output

        Serial.print("Pot ");
        Serial.print(i);
        Serial.print(" | ");
        Serial.print("PotState: ");
        Serial.print(potState[i]);
        Serial.print(" -  midiState: ");
        Serial.println(midiState[i]);

        midiPState[i] = midiState[i];  // Update the previous MIDI state
      }
      potPState[i] = potState[i];  // Update the previous potentiometer state
    }
  }
}

// Arduino (pro)micro MIDI functions using the MIDIUSB Library
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = { 0x09, 0x90 | channel, pitch, velocity };
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = { 0x08, 0x80 | channel, pitch, velocity };
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = { 0x0B, 0xB0 | channel, control, value };
  MidiUSB.sendMIDI(event);
}
