#include <Adafruit_NeoPixel.h>

#include <Adafruit_MCP2515.h>

#ifdef ESP8266
   #define CS_PIN    2
#elif defined(ESP32) && !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2) && !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S3)
   #define CS_PIN    14
#elif defined(TEENSYDUINO)
   #define CS_PIN    8
#elif defined(ARDUINO_STM32_FEATHER)
   #define CS_PIN    PC5
#elif defined(ARDUINO_NRF52832_FEATHER)  /* BSP 0.6.5 and higher! */
   #define CS_PIN    27
#elif defined(ARDUINO_MAX32620FTHR) || defined(ARDUINO_MAX32630FTHR)
   #define CS_PIN    P3_2
#elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040)
   #define CS_PIN    7
#elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040_CAN)
   #define CS_PIN    PIN_CAN_CS
#elif defined(ARDUINO_RASPBERRY_PI_PICO) || defined(ARDUINO_RASPBERRY_PI_PICO_W) // PiCowbell CAN Bus
   #define CS_PIN    20
#else
    // Anything else, defaults!
   #define CS_PIN    5
#endif

#define CAN_BAUDRATE (500000)

Adafruit_MCP2515 mcp(CS_PIN);

#define PIN        11
#define NUMPIXELS 12

Adafruit_NeoPixel pixels(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);
#define DELAYVAL 500

void setup() {
  pixels.begin();
  int i;
  for(i=0; i<18; i++) {

    pixels.clear();
    //Orange light show on startup
    if(i<9) pixels.setPixelColor(i, pixels.Color(200, 35, 0));
    else pixels.setPixelColor(17-i, pixels.Color(200, 35, 0));
    pixels.show();

    delay(50);
  }
  pixels.clear();
  pixels.show();
  delay(1000);
  if (!mcp.begin(CAN_BAUDRATE)) {

    while(1) delay(10);
  }
}

void loop() {
  pixels.clear();

  mcp.beginExtendedPacket(0x18db33f1, 8);
  mcp.write(0x02); // number of additional bytes
  mcp.write(0x01); // show current data
  mcp.write(0x0c); // engine RPM
  mcp.endPacket();

  while (mcp.parsePacket() == 0 ||
         mcp.read() < 3 ||          // correct length
         mcp.read() != 0x41 ||      // correct mode
         mcp.read() != 0x0c);       // correct PID
  
  int rpm = ((mcp.read() * 256.0) + mcp.read()) / 4.0;
  int numlights = rpm/1000;
  if(rpm < 1000) numlights = 1;
  if(rpm == 0) numlights = 0;
  int i;
  //RPM Lights
  for(i=0; i<numlights; i++) {
      if (i > 6) 
      {
        pixels.setPixelColor(i, pixels.Color(50, 0, 0));
      }
      else if (i > 4 && i < 7) 
      {
        pixels.setPixelColor(i, pixels.Color(50, 50, 0));
      }
      else{
        pixels.setPixelColor(i, pixels.Color(0, 50, 0));
      }
  }
  //redline
  if (rpm > 8800)
  {
    for(i=0; i<9; i++)
    {
      pixels.setPixelColor(i, pixels.Color(150, 0, 0));
    }
  }
pixels.show();
}

