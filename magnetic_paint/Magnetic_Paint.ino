#define FASTLED_INTERRUPT_RETRY_COUNT 0
#include <FastLED.h>
#include <Keypad.h>

#define DATA_PIN    13
#define LED_TYPE    WS2811
#define COLOR_ORDER GRB
#define NUM_LEDS    36
CRGB leds[NUM_LEDS];
byte BOARD_LEDS[] = {0, 1, 2, 3, 4, 5, 11, 10, 9, 8, 7, 6, 12, 13, 14, 15, 16, 17,
		23, 22, 21, 20, 19, 18, 24, 25, 26, 27, 28, 29, 35, 34, 33, 32, 31, 30};
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

byte palette_index;
int LED_index_list[36] = {0}; //list of saved indexes of color palette
CRGBPalette16 ledPal = RainbowColors_p; // could be HeatColors_p, PartyColors_p, RainbowColors_p, etc see http://fastled.io/docs/3.1/colorpalettes_8h_source.html

void setup() {
	Serial.begin(9600);
	// tell FastLED about the LED strip configuration
	FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
	// set master brightness control
	FastLED.setBrightness(BRIGHTNESS);
	fill_solid( leds, NUM_LEDS, CRGB(0,0,0));
	for (int i = 0; i < 36; i++) { //display the colors from palette
		leds[BOARD_LEDS[i]] = ColorFromPalette(ledPal, i*7);
		FastLED.show();
		FastLED.delay(30);
	}
	delay(2000);
	fade_to_black();
	kpd.setHoldTime(12);//setting the time after that the switches are considered closed continuously
} //just a little bit more then the default 10ms defined for debouncing

void loop() {
	// Fills kpd.key[ ] array with up-to 10 active keys.
	// Returns true if there are ANY active keys.
	kpd.getKeys();
		for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
		{
				if (kpd.key[i].kstate == HOLD) {
					palette_index = LED_index_list[kpd.key[i].kcode]; //get the actual LED color
					palette_index ++; //advance in color palette
					if (palette_index == 255) {palette_index = 0;} // at the end reinitialize the color index
					//Serial.println(palette_index++);
					leds[BOARD_LEDS[kpd.key[i].kcode]] = ColorFromPalette(ledPal, palette_index); //assign the new color from palette
					LED_index_list[kpd.key[i].kcode] = palette_index; //save this color in the
				}
	}
	FastLED.show();
}  // End loop

void fade_to_black() {
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
