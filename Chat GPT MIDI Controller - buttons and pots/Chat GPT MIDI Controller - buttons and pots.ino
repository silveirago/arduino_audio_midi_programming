#include <MIDIUSB.h>

const int buttonPin[] = {2, 3, 4};  // buttons connected to digital pins 2, 3, 4
const int buttonNotes[] = {60, 64, 67}; // notes to be sent when buttons are pressed
const int potPin[] = {A0, A1}; // potentiometer 1 connected to analog pin A0, potentiometer 2 connected to analog pin A1
const int controlNumber[] = {1, 2}; // MIDI control number for potentiometer 1 and 2
const int debounce = 50;    // debounce time in milliseconds
unsigned long lastButtonPress = 0;  // timestamp of last button press
unsigned long lastPotEvent[2] = {0}; // timestamp of last potentiometer event
int potValue[2] = {0};
bool buttonState[3] = {false}; //keeps track of button state
int potThreshold = 10; // threshold value for potentiometer movement
int potDuration = 300; // duration of control change messages in ms

#define MAX_NOTES 12 // max number of notes that can be played at the same time
int notesPlaying[MAX_NOTES] = {0};  // array to keep track of the notes that are currently playing
int notesPlayingIndex = 0; // index for the notesPlaying array

void setup() {
  //  MidiUSB.begin();
    for (int i = 0; i < 3; i++) {
        pinMode(buttonPin[i], INPUT_PULLUP);
    }
    for (int i = 0; i < 2; i++) {
        pinMode(potPin[i], INPUT);
    }
}

void loop() {
    handleButtons();
    handlePotentiometers();
}

void handleButtons() {
    for (int i = 0; i < 3; i++) {
        int currentButtonState = digitalRead(buttonPin[i]);
        if (currentButtonState == LOW && (millis() - lastButtonPress) > debounce) {
            if (!buttonState[i]) { // if button is not pressed
                midiEventPacket_t noteOn = {0x09, 0x90, buttonNotes[i], 100};
                MidiUSB.sendMIDI(noteOn);
                MidiUSB.flush();
                buttonState[i] = true; //update state
                notesPlaying[notesPlayingIndex] = buttonNotes[i]; // add the note to the notesPlaying array
                notesPlayingIndex++;
            }
            lastButtonPress = millis();
        } else if (currentButtonState == HIGH) {
            if (buttonState[i]) {
                // Send MIDI note off message
                midiEventPacket_t noteOff = {0x08, 0x80, buttonNotes[i], 0};
                MidiUSB.sendMIDI(noteOff);
                MidiUSB.flush();
                buttonState[i] = false; //update state
                //remove the note from the notesPlaying array
                for (int j = 0; j < notesPlayingIndex; j++) {
                    if (notesPlaying[j] == buttonNotes[i]) {
                        for (int k = j; k < notesPlayingIndex - 1; k++) {
                            notesPlaying[k] = notesPlaying[k + 1];
                        }
                        notesPlayingIndex--;
                        break;
                    }
                }
            }
        }
    }
}

void handlePotentiometers(){
    for (int i = 0; i < 2; i++) {
        int currentPotValue = analogRead(potPin[i]);
        if(abs(potValue[i] - currentPotValue) > potThreshold){
            lastPotEvent[i] = millis();
            potValue[i] = currentPotValue;
        }
        if((millis() - lastPotEvent[i]) < potDuration ){
            midiEventPacket_t controlChange = {0x0B, 0xB0, controlNumber[i], map(potValue[i], 0, 1023, 0, 127)};
            MidiUSB.sendMIDI(controlChange);
            MidiUSB.flush(); // send the MIDI message
        }
    }
}
