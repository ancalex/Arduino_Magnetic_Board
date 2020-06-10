#define FASTLED_INTERRUPT_RETRY_COUNT 0
#include <FastLED.h>
#include <Keypad.h>
#include "pitches.h"  //https://www.arduino.cc/en/Tutorial/toneMelody

#define DATA_PIN    13
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    36
CRGB leds[NUM_LEDS];
byte BOARD_LEDS[] = {0, 1, 2, 3, 4, 5, 11, 10, 9, 8, 7, 6, 12, 13, 14, 15, 16, 17,
		23, 22, 21, 20, 19, 18, 24, 25, 26, 27, 28, 29, 35, 34, 33, 32, 31, 30};
#define BRIGHTNESS	100

const byte ROWS = 6; //six rows
const byte COLS = 6; //six columns

//define the symbols on the buttons of the keypads
char keys[ROWS][COLS] = {
		{'0','1','2','3','4','5'},
		{'6','7','8','9','A','B'},
		{'C','D','E','F','G','H'},
		{'I','J','K','L','M','N'},
		{'O','P','Q','R','S','T'},
		{'U','V','W','X','Y','Z'},
};

byte rowPins[ROWS] = {A0, A1, 2, 4, 5, 6}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {7, 8, 9, 10, 11, 12}; //connect to the column pinouts of the keypad
Keypad kpd = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );
//the column that is verified for notes to be played
byte LED_column_index = 0;
//list of colors for notes ie  F3 will be blue, G3 will be red etc
CRGB LED_color[] = {CRGB::Blue, CRGB::Red, CRGB::Magenta, CRGB::Green, CRGB::Aqua, CRGB::Yellow};
//notes defined in pitches.h
int pentatonic_scale[] = {NOTE_F3, NOTE_G3, NOTE_A3, NOTE_C4, NOTE_D4, NOTE_F4}; // F Major
//int pentatonic_scale[] = {NOTE_C3, NOTE_D3, NOTE_E3, NOTE_G3, NOTE_A3, NOTE_C4}; // C Major
//list of saved notes
int LED_sound_list[36] = {0};

void setup() {
	Serial.begin(9600);
	// tell FastLED about the LED strip configuration
	FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
	// set master brightness control
	FastLED.setBrightness(BRIGHTNESS);
	fill_solid( leds, NUM_LEDS, CRGB(0,0,0)); // black all at start
}

void loop() {
	// Fills kpd.key[ ] array with up-to 10 active keys.
	// Returns true if there are ANY active keys.
	if (kpd.getKeys())
	{
		for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
		{
			if ( kpd.key[i].stateChanged )   // Only find keys that have changed state.
			{
				if (kpd.key[i].kstate == PRESSED) {
//					Serial.print("Key ");
//					Serial.print(kpd.key[i].kchar);
//					Serial.print(" - ");
//					Serial.println(kpd.key[i].kcode);
					if (LED_sound_list[kpd.key[i].kcode] == 0) { //if the LED is not lit
						leds[BOARD_LEDS[kpd.key[i].kcode]] = LED_color[int(kpd.key[i].kcode/6)]; //make it display the color from note color list
						LED_sound_list[kpd.key[i].kcode] = 1; //then save the new state
					}
					else {
						leds[BOARD_LEDS[kpd.key[i].kcode]] = CRGB::Black; //else if LED was previously lit make it black
						LED_sound_list[kpd.key[i].kcode] = 0; //save this state
					}
				}
			}
		}
	}

	for (int i = 0; i < 6; i++) { //for each note in the column
		if (LED_sound_list[i*6 + LED_column_index] == 0) { //if LED from column not lit
		leds[BOARD_LEDS[i*6 + LED_column_index]] = CRGB(4, 4, 4); //make it grey, to show the actual columns from where the notes are played
		}
		else {
		leds[BOARD_LEDS[i*6 + LED_column_index]].maximizeBrightness(255);//make the LED brighter
		//Serial.print("Note frequency: ");
		//Serial.println(pentatonic_scale[i]);
		tone(3, pentatonic_scale[i], 30);//play the note on pin3, frequency from scale, 30ms duration
		delay(39); //delay
		noTone(3); //stop the sound - play with these 3 durations for a good arpeggio effect
		}
	}
	FastLED.delay(200);
	for (int i = 0; i < 6; i++) {
			if ((LED_sound_list[i*6 + LED_column_index] == 0)) { // if LED from column not lit
			leds[BOARD_LEDS[i*6 + LED_column_index]] = CRGB::Black; //make it black because column index will advance
			}
			else {
			leds[BOARD_LEDS[i*6 + LED_column_index]].maximizeBrightness(BRIGHTNESS);//make LED default brightness
			}
		}
	LED_column_index++; //advance the column index
	if (LED_column_index == 6) {LED_column_index = 0;}// if index at the 6th column make it zero again
	FastLED.show();
}  // End loop
