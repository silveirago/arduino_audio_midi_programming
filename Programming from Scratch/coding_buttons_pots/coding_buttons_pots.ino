/*
by Gustavo Silveira | aka Nerd Musician
-> Nerd Musician Pro: https://www.musiconerd.com/nerd-musician-pro/
-> Curso Fazendo Música com Arduino em Português: https://www.musiconerd.com/curso-completo
gustavosilveira@musiconerd.com
*/

// - - - - - - - - - - - - - - - - - - - - - - - - -

#include "MIDIUSB.h"

// BUTTONS
const int N_BUTTON = 3;                  // total number of buttons
byte buttonPin[N_BUTTON] = { 2, 3, 4 };  // button pins
byte buttonState[N_BUTTON] = { 0 };      // button state
byte buttonPState[N_BUTTON] = { 0 };     // button previous state

int debounceDelay = 10;  // 10 ms
unsigned long lastDebounceTime[N_BUTTON] = { 0 };
unsigned long debounceTimer[N_BUTTON] = { 0 };

// - - - - - - - - - - - - - - - - - - - - - - - - -

// POTENTIOMETERS
const int N_POT = 2;
byte potPin[N_POT] = { A0, A1 };
int potState[N_POT] = { 0 };
int potPState[N_POT] = { 0 };
byte midiState[N_POT] = { 0 };
byte midiPState[N_POT] = { 0 };

int varThreshold = 20;  // how much it need to vary to send a message

unsigned long potTimer[N_POT] = { 0 };
unsigned long lastPotTime[N_POT] = { 0 };
const int POT_TIMEOUT = 300;
boolean potGate[N_POT] = { false };

// - - - - - - - - - - - - - - - - - - - - - - - - -

byte MIDI_CH = 0;
byte NN[N_BUTTON] = { 36, 38, 40 };  // note list
byte CC[N_BUTTON] = { 10, 11 };      // CC list

void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);  // starts serial monitor

  for (int i = 0; i < N_BUTTON; i++) {
    pinMode(buttonPin[i], INPUT_PULLUP);  // declare the button pins
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - -

void loop() {
  // put your main code here, to run repeatedly:

  buttons();
  potentiometers();
}

// - - - - - - - - - - - - - - - - - - - - - - - - -

void buttons() {

  for (int i = 0; i < N_BUTTON; i++) {
    buttonState[i] = digitalRead(buttonPin[i]);

    // we only want to send a message if the message now is different from before

    debounceTimer[i] = millis() - lastDebounceTime[i];

    if (debounceTimer[i] > debounceDelay) {

      if (buttonState[i] != buttonPState[i]) {

        // start timer
        lastDebounceTime[i] = millis();

        if (buttonState[i] == LOW) {
          // note on

          noteOn(MIDI_CH, NN[i], 127);
          MidiUSB.flush();

          Serial.print("Button ");
          Serial.print(i);
          Serial.print(": on");
          Serial.println();

        } else {
          // note off

          noteOn(MIDI_CH, NN[i], 0);
          MidiUSB.flush();

          Serial.print("Button ");
          Serial.print(i);
          Serial.print(": off");
          Serial.println();
        }

        buttonPState[i] = buttonState[i];
      }
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - -

void potentiometers() {

  for (int i = 0; i < N_POT; i++) {
    // reads all the pots
    potState[i] = analogRead(potPin[i]);  // stores the analog val
    midiState[i] = map(potState[i], 0, 1023, 0, 127);

    int potVar = abs(potState[i] - potPState[i]);  // variation between the previous and the current val
    //Serial.println(potVar);

    if (potVar > varThreshold) {
      lastPotTime[i] = millis();
    }

    potTimer[i] = millis() - lastPotTime[i];

    if (potTimer[i] < POT_TIMEOUT) {
      potGate[i] = true;
    } else {
      potGate[i] = false;
    }

    if (potGate[i] = true) {
      if (midiState[i] != midiPState[i]) {

        controlChange(MIDI_CH, CC[i], midiState[i]); // sends control change

        Serial.print("Pot ");
        Serial.print(i);
        Serial.print(": ");
        Serial.print(midiState[i]);
        Serial.println();

        midiPState[i] = midiState[i];
        potPState[i] = potState[i];
      }
    }
  }
}

// - - - - - - - - - - - - - - - - - - - - - - - - -

// Arduino (pro)micro midi functions MIDIUSB Library
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = { 0x09, 0x90 | channel, pitch, velocity };
  MidiUSB.sendMIDI(noteOn);
}

void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = { 0x0B, 0xB0 | channel, control, value };
  MidiUSB.sendMIDI(event);
}