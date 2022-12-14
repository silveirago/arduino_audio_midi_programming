#include <MIDIUSB.h>

byte randNote;
byte note[8] = { 36, 38, 40, 42, 44, 46, 48, 50 };

void setup() {
  // put your setup code here, to run once:
}

void loop() {

  randNote = random(8);

  noteOn(0, note[randNote], 127);
  MidiUSB.flush();
  delay(random(50, 200));
  noteOn(0, note[randNote], 0);
  MidiUSB.flush();
  delay(random(50, 200));

}

/////////////////////////////////////////////
// Arduino (pro)micro midi functions MIDIUSB Library
void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = { 0x09, 0x90 | channel, pitch, velocity };
  MidiUSB.sendMIDI(noteOn);
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

void pitchBend(byte channel, int value) {
  byte lowValue = value & 0x7F;
  byte highValue = value >> 7;
  midiEventPacket_t pitchBend = { 0x0E, 0xE0 | channel, lowValue, highValue };
  MidiUSB.sendMIDI(pitchBend);
}