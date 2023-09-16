/*
  This code reads the Arduino digital ports (buttons) and sends MIDI notes.
Tutorial: https://youtu.be/IcjKv6ZTiHI

by Gustavo Silveira, a.k.a. Nerd Musician
Learn how to builf your MIDI controllers: https://go.musiconerd.com/
*/

#include "MIDIUSB.h" // Include the MIDIUSB library for MIDI communication

// Define the number of buttons
const int N_BUTTONS = 3;

// Define the Arduino pins for buttons and their corresponding MIDI note numbers
int buttonPin[N_BUTTONS] = { 4, 3, 2 };
int buttonNote[N_BUTTONS] = { 36, 38, 42 };

// Arrays to store the current and previous button states
int buttonState[N_BUTTONS] = { 0 };
int buttonPState[N_BUTTONS] = { 0 };  // previous state of the button

// Arrays to manage button debouncing
unsigned long lastDebounceTime[N_BUTTONS] = { 0 };
unsigned long debounceTimer[N_BUTTONS] = { 0 };
int debounceDelay = 10;

int BUTTON_CH = 0; // MIDI channel for button events

void setup() {
  Serial.begin(9600); // Initialize serial communication for debugging

  // Configure each button pin as INPUT_PULLUP
  for (int i = 0; i < N_BUTTONS; i++) {
    pinMode(buttonPin[i], INPUT_PULLUP);
  }

  // Additional setup code can be added here
}

void loop() {
  buttons(); // Call the function to handle button events
}

// Function to read and handle button states
void buttons() {
  for (int i = 0; i < N_BUTTONS; i++) {
    buttonState[i] = digitalRead(buttonPin[i]); // Read the current button state

    debounceTimer[i] = millis() - lastDebounceTime[i]; // Calculate time since last state change

    // Check if enough time has passed for debounce
    if (debounceTimer[i] > debounceDelay) {
      if (buttonState[i] != buttonPState[i]) { // Check if button state has changed
        lastDebounceTime[i] = millis(); // Update the debounce time

        // Check if the button is pressed
        if (buttonState[i] == LOW) {
          noteOn(BUTTON_CH, buttonNote[i], 127); // Send MIDI note-on message
          MidiUSB.flush(); // Flush the MIDI data

          Serial.print("Button ");
          Serial.print(i);
          Serial.print(" ");
          Serial.println("on");
        } else { // Button is released
          noteOn(BUTTON_CH, buttonNote[i], 0); // Send MIDI note-off message
          MidiUSB.flush(); // Flush the MIDI data

          Serial.print("Button ");
          Serial.print(i);
          Serial.print(" ");
          Serial.println("off");
        }

        buttonPState[i] = buttonState[i]; // Update the previous button state
      }
    }
  }
}

// Function to send a MIDI note-on message
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = { 0x09, 0x90 | channel, pitch, velocity };
  MidiUSB.sendMIDI(noteOn);
}

// Function to send a MIDI control change message (not used in this code)
void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = { 0x0B, 0xB0 | channel, control, value };
  MidiUSB.sendMIDI(event);
}