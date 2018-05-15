
#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "distance.h"
#include "i2cmaster.h"
#include "millis.h"
#include "VL53L0X.h"
#include <util/delay.h>
#include "VL53L0X.h"

/**************** The HT16K33 part ********************/
#define HT16K33_ADDRESS 0xE0
#define HT16K33_OSCILLATOR_ON 0x21
#define HT16K33_BLINK_CMD 0x80
#define HT16K33_BLINK_DISPLAYON 0x01
#define HT16K33_BRIGHTNESS 0xE0

// Our display buffer
uint16_t displaybuffer[8];

// Display ON and no blink
void initNoBlink() {
    i2c_start(HT16K33_ADDRESS);
    i2c_write(HT16K33_BLINK_CMD | HT16K33_BLINK_DISPLAYON); //0x81
    i2c_stop();
}
// Set the brightness (between 0 and 15)
void setBrightness(uint8_t b) {
    if (b>15) b = 15;
    i2c_start(HT16K33_ADDRESS);
    i2c_write(0xE0 | b);
    i2c_stop();
}
// Oscillator ON, no blink, brightness set to 1
void initHt16k33(void) {
    i2c_start(HT16K33_ADDRESS);
    i2c_write(HT16K33_OSCILLATOR_ON);
    i2c_stop();

    initNoBlink();
    setBrightness(1);
}
// Switch led (x,y) ON in the buffer (need to call display to see the result)
void setPixel(int16_t x, int16_t y) {
    x += 7;
    x %= 8;
    displaybuffer[y] |= 1 << x;
}
// Switch led (x,y) OFF in the buffer (need to call display to see the result)
void unsetPixel(int16_t x, int16_t y) {
    x += 7;
    x %= 8;
    displaybuffer[y] &= ~(1 << x);
}
// Switch ON n pixels line l
void setLine(uint8_t l, uint8_t n) {
    // unset the line
    unsetAllLine(l);
    // set the pixels
    for (uint8_t i = 0 ; i < n ; i++) {
        setPixel(i, l);
    }
}
void unsetAllLine(uint8_t l) {
    for (uint8_t i = 0 ; i < 8 ; i++) {
        unsetPixel(i, l);
    }
}
void shiftAllLine() {
    uint16_t temp;

    displaybuffer[0] = displaybuffer [1];
    displaybuffer[1] = displaybuffer [2];
    displaybuffer[2] = displaybuffer [3];
    displaybuffer[3] = displaybuffer [4];
    displaybuffer[4] = displaybuffer [5];
    displaybuffer[5] = displaybuffer [6];
    displaybuffer[6] = displaybuffer [7];
    
}
// Trigger a display of the buffer
void display() {
    uint8_t i;
    i2c_start(HT16K33_ADDRESS);
    i2c_write(0x00);
    for (i=0; i<8; i++) {
        i2c_write(displaybuffer[i] & 0xFF);    
        i2c_write(displaybuffer[i] >> 8);
    }
    i2c_stop();
}

/**************** Main functions ********************/

void initAll(void) {
    //--------------------------------------------------
	// GPIOs
	//--------------------------------------------------
	CBI( UCSR0B, RXEN0 );		// Disable UART RX
	DDRD =  (1<<PIN_UART_TX);	// Set UART TX as output
	// Enable weak pullups on I2C lines
	PORTC = (1<<PIN_I2C_SCL) | (1<<PIN_I2C_SDA);
	//--------------------------------------------------
	// Init the other modules
	//--------------------------------------------------
	i2c_init();
	initMillis();
	sei();
    // LED matrix
    initHt16k33();
    // Init laser sensor
    initVL53L0X(1);
}

// Main loop where we display on and off LEDs every second
int main(void) {
    statInfo_t xTraStats;
    uint16_t d;
	
    initAll();

    // Let the HT16K33 initialise
    _delay_ms(10);

    setMeasurementTimingBudget( 500 * 1000UL );		// integrate over 500 ms per measurement

    // Switch on a LED every second
    while(1) {
        readRangeSingleMillimeters( &xTraStats );
        d = xTraStats.rawDistance;

        // Set the max distance to 40 cm
        if (d >= 400) {
            d = 400;
        }

        //shift all the display
        shiftAllLine();

        // set the pixels on line 8
        setLine(7, (8 - (d/50)));
        display();        
    }
    return 0;
}