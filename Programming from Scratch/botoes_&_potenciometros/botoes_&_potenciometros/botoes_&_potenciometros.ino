// Arduino Pro Micro usa o ATmega32U4 (Leonardo, Micro, Pro Micro)
#include "MIDIUSB.h"

// BOTOES
const int N_BUTTON = 3;                       // numero total de botoes
byte buttonPin[N_BUTTON] = { 2, 3, 4 };       // pinos digitais
byte buttonState[N_BUTTON] = { 0 };           // estado atual
byte buttonPState[N_BUTTON] = { 0 };          // estado previo
unsigned long lastBounce[N_BUTTON] = { 0 };   // para "resetar" o relogio
unsigned long buttonTimer[N_BUTTON] = { 0 };  // timer do botao
int buttonTimeout = 15;

// - - - - - - - - - - - - - -

// POTENCIOMETROS
const int N_POTS = 2;              // numero total de pots
byte potPin[N_POTS] = { A0, A1 };  // pinos analogicos
int potState[N_POTS] = { 0 };
int potPState[N_POTS] = { 0 };
byte midiState[N_POTS] = { 0 };
byte midiPState[N_POTS] = { 0 };
int varThreshold = 20;

boolean openGate[N_POTS] = { false };

unsigned long lastPot[N_POTS] = { 0 };
unsigned long potTimer[N_POTS] = { 0 };

int potTimeout = 300;

int MIDI_CH = 0;  // 0-15
int NN[N_BUTTON] = { 36, 38, 40 };
int CC[N_POTS] = { 11, 13 };

void setup() {

  Serial.begin(115200);

  // delclarar os pinos do arduino
  for (int i = 0; i < N_BUTTON; i++) {
    pinMode(buttonPin[i], INPUT_PULLUP);
  }
}

// - - - - - - - - - - - - - -

void loop() {
  // put your main code here, to run repeatedly:

  buttons();
  potentiometers();
}

// - - - - - - - - - - - - - -

void buttons() {

  for (int i = 0; i < N_BUTTON; i++) {
    buttonState[i] = digitalRead(buttonPin[i]);

    buttonTimer[i] = millis() - lastBounce[i];  // corre o timer

    if (buttonTimer[i] > buttonTimeout) {

      if (buttonState[i] != buttonPState[i]) {
        lastBounce[i] = millis();  // armazena a hora

        if (buttonState[i] == 0) {
          // mandar note on
          noteOn(MIDI_CH, NN[i], 127);
          MidiUSB.flush();

          Serial.print("Botao ");
          Serial.print(i);
          Serial.println(": on");
        } else {
          // mandar note off
          noteOn(MIDI_CH, NN[i], 0);
          MidiUSB.flush();

          Serial.print("Botao ");
          Serial.print(i);
          Serial.println(": off");
        }

        buttonPState[i] = buttonState[i];
      }
    }
  }
}

// - - - - - - - - - - - - - -

void potentiometers() {

  for (int i = 0; i < N_POTS; i++) {
    potState[i] = analogRead(potPin[i]);  // leio o estado do pot | 0-1023
    midiState[i] = map(potState[i], 0, 1020, 0, 127);

    int potVar = abs(potState[i] - potPState[i]);

    if (potVar > varThreshold) {
      lastPot[i] = millis();  // salva que horas sao
    }

    potTimer[i] = millis() - lastPot[i];  // corre o timer

    if (potTimer[i] < potTimeout) {
      if (midiState[i] != midiPState[i]) {  // estado atual for diferente do previo
        // mandar CC
        controlChange(MIDI_CH, CC[i], midiState[i]);
        MidiUSB.flush();

        Serial.print("Pot ");
        Serial.print(i);
        Serial.print(": ");
        Serial.println(midiState[i]);

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