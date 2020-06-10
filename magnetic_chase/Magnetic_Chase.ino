#define FASTLED_INTERRUPT_RETRY_COUNT 0
#include <Keypad.h>
#include "FastLED.h"

const byte ROWS = 6; //six rows
const byte COLS = 6; //six columns
//define the 'symbols' of the reed switches of the board but the number in the
//switches list - kcode - will be used
char keys[ROWS][COLS] = {
		{'0','1','2','3','4','5'},
		{'6','7','8','9','A','B'},
		{'C','D','E','F','G','H'},
		{'I','J','K','L','M','N'},
		{'O','P','Q','R','S','T'},
		{'U','V','W','X','Y','Z'},
};
byte rowPins[ROWS] = {A0, A1, 2, 4, 5, 6}; //connect to the row pinouts of the board
byte colPins[COLS] = {7, 8, 9, 10, 11, 12}; //connect to the column pinouts of the board
Keypad board = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

#define LED_TYPE WS2811
#define COLOR_ORDER GRB
#define LED_PIN 13
#define NUM_LEDS  60
#define BRIGHTNESS 160
CRGB leds[NUM_LEDS];
byte BOARD_LEDS[] = {0, 1, 2, 3, 4, 5, 11, 10, 9, 8, 7, 6, 12, 13, 14, 15, 16, 17,
		23, 22, 21, 20, 19, 18, 24, 25, 26, 27, 28, 29, 35, 34, 33, 32, 31, 30};
int loops = 0;
int random_LED = 0;
int game_start = 1;
int game_end = 0;
int random_LED_list[36];
CRGBPalette16 ledPal = ForestColors_p; // could be RainbowColors_p, PartyColors_p, OceanColors_p, etc see http://fastled.io/docs/3.1/colorpalettes_8h_source.html

void setup() {
	Serial.begin(9600);
	FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalSMD5050);
	FastLED.setBrightness(BRIGHTNESS);
	Serial.println("FastLed Setup done");
	fill_solid( leds, NUM_LEDS, CRGB(0,0,0)); //all LEDs to black at start
}

void loop() {
	// Fills board.key[ ] array with up-to 10 active switches.
	// Returns true if there are ANY active switches.
	if (game_start == 1) { //game is starting
		for (int i = 0; i < 36; i++) { //initialize the LED list to be 'chased, in order from 0 to 35
			random_LED_list[i] = i;
		}
		int seed = analogRead(2); //try to randomize a little bit
		randomSeed(seed);
		for (int i = 0; i < 36; i++) { //shuffle the LED list to be 'chased'
			int j = random(0,i);  // https://www.rosettacode.org/wiki/Knuth_shuffle
			int temp = random_LED_list[i];
			random_LED_list[i]= random_LED_list[j];
			random_LED_list[j] = temp;
		}
		for (int i = 0; i < 36; i++) { //show on display the LEDs in the shuffled order
			leds[BOARD_LEDS[random_LED_list[i]]] = ColorFromPalette(ledPal, i*7); // colored as from the predefined color palette
			FastLED.show();
			FastLED.delay(30);
			beep_effect();
		}
		fade_to_black(); // fade slowly
		random_LED = random_LED_list[0]; //take the first LED
		leds[BOARD_LEDS[random_LED]] = CRGB::Red; //lit it to RED
		game_start = 0; //initialize
		loops = 0;
	}
	if (game_end == 1) { // at game end beep a little bit fade the LEDs and make start again
		for( int i = 0; i < 20; i++){
			beep_effect();
		}
		fade_to_black();
		game_end = 0;
		game_start = 1;
	}
	if (board.getKeys())
	{
		for (int i=0; i<LIST_MAX; i++)   // Scan the whole active switch list.
		{
			if ( board.key[i].stateChanged )   // Only find switches that have changed state.
			{
				if (board.key[i].kstate == PRESSED) {
					//Serial.println(board.key[i].kcode);
					if (board.key[i].kcode == random_LED_list[loops]) // if LED 'caught'
					{
						leds[BOARD_LEDS[random_LED_list[loops]]] = ColorFromPalette(ledPal, loops*7); //lit the LED to the color from color palette
						beep_effect();
						//beep_effect();
						FastLED.show();
						loops ++; // counting till the end of LEDs to be 'chased'
						if (loops == 36) { // if all LED has been 'caught'
							game_end = 1;
						}
						else {
							leds[BOARD_LEDS[random_LED_list[loops]]] = CRGB::Red; // next LED to 'catch'
						}
						FastLED.show();
					}
				}
			}
		}
	}
	FastLED.show();
}  // End loop

void fade_to_black() { //slowly fade the LEDs to black
	for (int j = 0; j < 64; j++) {
		for (int i = 0; i < 36; i++) {
			leds[i].fadeToBlackBy(j);
		}
		FastLED.show();
		FastLED.delay(20);
	}
	fill_solid( leds, NUM_LEDS, CRGB(0,0,0));
	FastLED.show();
}

void beep_effect() { //game start effect
	for( int i = 1; i < 800; i += 2){
		tone(3, 400+i, 2);
	}
}
