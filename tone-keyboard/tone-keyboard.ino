
/*
  Keyboard

  Plays a pitch that changes based on a changing
  input circuit:

  * 8-ohm speaker on digital pin 8
*/
const int noteN = 7;
int buttonPin[noteN] = { 2, 3, 4, 5, 6, 7, 8 };
int note[noteN] = { 0, 2, 4, 5, 7, 9, 11 };
int octave = 6 * 12;
int buttonState[noteN];

int speakerPin = 9;

boolean isPlaying = false;
int counter = 0;


void setup() {

  Serial.begin(115200);

  pinMode(speakerPin, OUTPUT);

  for (int i = 0; i < noteN; i++) {
    pinMode(buttonPin[i], INPUT_PULLUP);
  }
}

void loop() {
  // tone(8, 440, 100);  // play tone 57 (A4 = 440 Hz)


  for (int i = 0; i < noteN; i++) {
    buttonState[i] = digitalRead(buttonPin[i]);
    if (buttonState[i] == LOW) {
      isPlaying = true;
    } else if (buttonState[0] && buttonState[1] && buttonState[2] && buttonState[3] && buttonState[4] && buttonState[5] && buttonState[6] == HIGH) {
      isPlaying = false;
    }
  }

  for (int i = 0; i < noteN; i++) {
    if (buttonState[i] == LOW) {
      // play a note
      tone(speakerPin, midiToFreq(note[i] + octave));
    }
    if (isPlaying == false) {
      noTone(speakerPin);      
    }
  }
}

float midiToFreq(float midival) {
  float f = 0.0;
  if (midival) f = 8.1757989156 * pow(2.0, midival / 12.0);
  return f;
}