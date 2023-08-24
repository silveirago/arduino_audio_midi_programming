/*
This code reads the Arduino digital ports (buttons) and sends MIDI notes.
by Gustavo Silveira, a.k.a. the Nerd Musician.
Visit: https://go.musiconerd.com/nerd-musician-pro
*/

#include "MIDIUSB.h"

const int N_BUTTONS = 3;

int buttonPin[N_BUTTONS] = { 4, 3, 2 };
int buttonNote[N_BUTTONS] = { 36, 38, 42 };

int buttonState[N_BUTTONS] = { 0 };
int buttonPState[N_BUTTONS] = { 0 };  // previous state of the button

unsigned long lastDebounceTime[N_BUTTONS] = { 0 };
unsigned long debounceTimer[N_BUTTONS] = { 0 };
int debounceDelay = 10;

int BUTTON_CH = 0;


void setup() {

  Serial.begin(9600);

  for (int i = 0; i < N_BUTTONS; i++) {

    pinMode(buttonPin[i], INPUT_PULLUP);
  }


  // put your setup code here, to run once:
}

void loop() {

  buttons();
}

// - - - - - - - - - - - - - - - - - - - - - - - -

void buttons() {

  for (int i = 0; i < N_BUTTONS; i++) {

    buttonState[i] = digitalRead(buttonPin[i]);

    debounceTimer[i] = millis() - lastDebounceTime[i];

    if (debounceTimer[i] > debounceDelay) {

      if (buttonState[i] != buttonPState[i]) {

        lastDebounceTime[i] = millis();

        if (buttonState[i] == LOW) {

          noteOn(BUTTON_CH, buttonNote[i], 127);
          MidiUSB.flush();

          Serial.print("Button ");
          Serial.print(i);
          Serial.print(" ");
          Serial.println("on");
        } else {

          noteOn(BUTTON_CH, buttonNote[i], 0);
          MidiUSB.flush();

          Serial.print("Button ");
          Serial.print(i);
          Serial.print(" ");
          Serial.println("off");
        }

        buttonPState[i] = buttonState[i];
      }
    }
  }
}
// - - - - - - - - - - - - - - - - - - - - - - - -

// Arduino (pro)micro midi functions MIDIUSB Library
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = { 0x09, 0x90 | channel, pitch, velocity };
  MidiUSB.sendMIDI(noteOn);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = { 0x0B, 0xB0 | channel, control, value };
  MidiUSB.sendMIDI(event);
}