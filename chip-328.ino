#include "chip328lib.h"
#include "ssd1306.h"
#include <Keypad.h>

const byte ROWS = 4;
const byte COLS = 4;
uint8_t lastKey;

uint8_t keys[ROWS][COLS] = {
  {0xC,0xD,0xE,0xF},
  {0x3,0x6,0x9,0xB},
  {0x2,0x5,0x8,0x0},
  {0x1,0x4,0x7,0xA}
};

byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {6, 7, 8, 9}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

uint8_t interfaceGetKey(){
  return keypad.getKey();
}

unsigned long current = 0, dtMillis = 0;

uint8_t chip328Display[256];

uint8_t interfacePutPixel(uint8_t x, uint8_t y, uint8_t pixel){
  uint8_t collision = 0;
  uint8_t pos;

  if(pixel){
    pos = ((x+(y/8)*64));

    if(chip328Display[pos]&(0b00000001<<(y%8))) collision = 1;
    chip328Display[pos] = chip328Display[pos]^(0b00000001<<(y%8));

  }

  uint8_t original = chip328Display[x+(y/8)*64];
  uint8_t expandedUp = 0b00000000, expandedDown = 0b00000000;

  expandedUp = expandedUp | (original&0b00010000)>>4;
  expandedUp = expandedUp | (original&0b00010000)>>3;
  expandedUp = expandedUp | (original&0b00100000)>>3;
  expandedUp = expandedUp | (original&0b00100000)>>2;
  expandedUp = expandedUp | (original&0b01000000)>>2;
  expandedUp = expandedUp | (original&0b01000000)>>1;
  expandedUp = expandedUp | (original&0b10000000)>>1;
  expandedUp = expandedUp | (original&0b10000000);

  expandedDown = expandedDown | (original&0b00000001);
  expandedDown = expandedDown | (original&0b00000001)<<1;
  expandedDown = expandedDown | (original&0b00000010)<<1;
  expandedDown = expandedDown | (original&0b00000010)<<2;
  expandedDown = expandedDown | (original&0b00000100)<<2;
  expandedDown = expandedDown | (original&0b00000100)<<3;
  expandedDown = expandedDown | (original&0b00001000)<<3;
  expandedDown = expandedDown | (original&0b00001000)<<4;

  //O MSB do display é o LSB do buffer...
  ssd1306_putPixels(x*2, ((y/8)*8)*2, expandedDown);
  ssd1306_putPixels(x*2+1, ((y/8)*8)*2, expandedDown);

  ssd1306_putPixels(x*2, ((y/8)*8)*2+8, expandedUp);
  ssd1306_putPixels(x*2+1, ((y/8)*8)*2+8, expandedUp);

  return collision;
}

void interfaceDelayTimer(int value){
  //if(DT!=0) DT--;
  //glutTimerFunc(17, interfaceDelayTimer, 0);
}

void interfaceLoadROM(){
  int i;
  const PROGMEM uint8_t brix[280] = {0x6E, 0x05, 0x65, 0x00, 0x6B, 0x06, 0x6A, 0x00, 0xA3, 0x0C, 0xDA, 0xB1, 0x7A, 0x04, 0x3A, 0x40, 0x12, 0x08, 0x7B, 0x02, 0x3B, 0x12, 0x12, 0x06, 0x6C, 0x20, 0x6D, 0x1F, 0xA3, 0x10, 0xDC, 0xD1, 0x22, 0xF6, 0x60, 0x00, 0x61, 0x00, 0xA3, 0x12, 0xD0, 0x11, 0x70, 0x08, 0xA3, 0x0E, 0xD0, 0x11, 0x60, 0x40, 0xF0, 0x15, 0xF0, 0x07, 0x30, 0x00, 0x12, 0x34, 0xC6, 0x0F, 0x67, 0x1E, 0x68, 0x01, 0x69, 0xFF, 0xA3, 0x0E, 0xD6, 0x71, 0xA3, 0x10, 0xDC, 0xD1, 0x60, 0x04, 0xE0, 0xA1, 0x7C, 0xFE, 0x60, 0x06, 0xE0, 0xA1, 0x7C, 0x02, 0x60, 0x3F, 0x8C, 0x02, 0xDC, 0xD1, 0xA3, 0x0E, 0xD6, 0x71, 0x86, 0x84, 0x87, 0x94, 0x60, 0x3F, 0x86, 0x02, 0x61, 0x1F, 0x87, 0x12, 0x47, 0x1F, 0x12, 0xAC, 0x46, 0x00, 0x68, 0x01, 0x46, 0x3F, 0x68, 0xFF, 0x47, 0x00, 0x69, 0x01, 0xD6, 0x71, 0x3F, 0x01, 0x12, 0xAA, 0x47, 0x1F, 0x12, 0xAA, 0x60, 0x05, 0x80, 0x75, 0x3F, 0x00, 0x12, 0xAA, 0x60, 0x01, 0xF0, 0x18, 0x80, 0x60, 0x61, 0xFC, 0x80, 0x12, 0xA3, 0x0C, 0xD0, 0x71, 0x60, 0xFE, 0x89, 0x03, 0x22, 0xF6, 0x75, 0x01, 0x22, 0xF6, 0x45, 0x60, 0x12, 0xDE, 0x12, 0x46, 0x69, 0xFF, 0x80, 0x60, 0x80, 0xC5, 0x3F, 0x01, 0x12 , 0xCA, 0x61, 0x02, 0x80, 0x15, 0x3F, 0x01, 0x12, 0xE0, 0x80, 0x15, 0x3F, 0x01, 0x12, 0xEE, 0x80, 0x15, 0x3F, 0x01, 0x12, 0xE8, 0x60, 0x20, 0xF0, 0x18, 0xA3, 0x0E, 0x7E, 0xFF, 0x80, 0xE0, 0x80, 0x04, 0x61, 0x00, 0xD0, 0x11, 0x3E, 0x00, 0x12, 0x30, 0x12, 0xDE, 0x78, 0xFF, 0x48, 0xFE, 0x68 , 0xFF, 0x12, 0xEE, 0x78, 0x01, 0x48, 0x02, 0x68, 0x01, 0x60, 0x04, 0xF0, 0x18, 0x69, 0xFF, 0x12, 0x70, 0xA3, 0x14, 0xF5, 0x33, 0xF2, 0x65, 0xF1, 0x29, 0x63, 0x37, 0x64, 0x00, 0xD3, 0x45, 0x73, 0x05, 0xF2, 0x29, 0xD3, 0x45, 0x00, 0xEE, 0xE0, 0x00, 0x80, 0x00, 0xFC, 0x00, 0xAA, 0x00, 0x00, 0x00, 0x00, 0x00};
  for(i=0;i<280;i++){
    chip328MemoryWrite(i+512, brix[i]);
  }
}

void setup() {
    Serial.begin(9600);
    //Serial.println("Setup");
    interfaceLoadROM();
    chip328Begin();
    ssd1306_128x64_i2c_init();
    ssd1306_fillScreen( 0x00 );
    Serial.println("End of setup");
}

void loop() {
    chip328Emulate();
    DT = 0;
}
