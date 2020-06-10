#define FASTLED_INTERRUPT_RETRY_COUNT 0
#include <FastLED.h>
#include <Keypad.h>

#define DATA_PIN    13			//Digital Out for controlling the LEDs
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    36
CRGB leds[NUM_LEDS];
byte BOARD_LEDS[] = {0, 1, 2, 3, 4, 5, 11, 10, 9, 8, 7, 6, 12, 13, 14, 15, 16, 17,
		23, 22, 21, 20, 19, 18, 24, 25, 26, 27, 28, 29, 35, 34, 33, 32, 31, 30}; //order of LEDs as the reed switches under them
#define BRIGHTNESS	128

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

void setup() {
	Serial.begin(9600);
	// tell FastLED about the LED strip configuration
	FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
	// set master brightness control
	FastLED.setBrightness(BRIGHTNESS);
}

void loop() {
	// Fills kpd.key[ ] array with up-to 10 active keys - I modified to 23 keys
	// Returns true if there are ANY active keys.
	if (kpd.getKeys())
	{
		for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
		{
			if ( kpd.key[i].stateChanged )   // Only find keys that have changed state.
			{
				if (kpd.key[i].kstate == PRESSED) {
					Serial.print("Key ");
					Serial.print(kpd.key[i].kchar);
					Serial.print(" - ");
					Serial.println(kpd.key[i].kcode);
					leds[BOARD_LEDS[kpd.key[i].kcode]] = CRGB::Red; //if the reed switch is closed lit the LED above
				}
				else if (kpd.key[i].kstate == RELEASED) {
					leds[BOARD_LEDS[kpd.key[i].kcode]] = CRGB::Black; //if the reed switch is opened stop the LED light
				}
			}
		}
	}
	FastLED.show();
}  // End loop
