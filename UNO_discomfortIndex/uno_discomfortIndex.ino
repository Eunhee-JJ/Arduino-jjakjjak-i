#include <SoftwareSerial.h>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_TFTLCD.h> // Hardware-specific library
#include <avr/pgmspace.h>
#include <SPI.h>
#include <SD.h>
//#include "photo.h"

SoftwareSerial MS(A5,14);

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

#define SD_CS 10  //SD card pin on your shield

#define BUFFPIXEL 20

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);



//float q;

String strData = "";
String strTem = "";
String strHum = "";

int idx_t=0, idx_h=0;

int cur = 0;
int next = 0;


void setup(){
  Serial.begin(9600);
  MS.begin(9600);
  pinMode(10, OUTPUT);
  digitalWrite(10, HIGH);
  if (!SD.begin(SD_CS)) {
  progmemPrintln(PSTR("failed!"));
  return;
  }

  //Serial.println(F("TFT LCD test"));
  //Serial.print(F("TFT size is ")); Serial.print(tft.width()); Serial.print(F("x")); Serial.println(tft.height());
  // 240x320

  tft.reset();

  uint16_t identifier = 0x9341;

  /*if(identifier == 0x9325) {
    //Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {
    //Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x7575) {
    //Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {
    //Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {
    //Serial.println(F("Found HX8357D LCD driver"));
  } else {
    //Serial.print(F("Unknown LCD driver chip: "));
    //Serial.println(identifier, HEX);
    //Serial.println(F("If using the Adafruit 2.8\" TFT Arduino shield, the line:"));
    //Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
    //Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
    //Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    //Serial.println(F("Also if using the breakout, double-check that all wiring"));
    //Serial.println(F("matches the tutorial."));
    //return;
    
  }*/

  tft.begin(identifier);
  tft.setRotation(1);
  tft.fillScreen(BLACK);
  //tft.fillRect(0,0,320,34,BLUE);
  tft.drawRect(2,35,99,200,WHITE);
  tft.drawRect(100,35,109,200,WHITE);
  tft.drawRect(208,35,109,200,WHITE);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.setCursor(8,5);
  tft.print("All in One");
  //clearMonitor();
  //tft.drawGrayscaleBitmap(11,80,good,80,99);
  
}


void loop(){
  delay(2153);
  TH();
  
}


void clearMonitor()
{
  //tft.fillRect(2,35,99,200,WHITE);
  tft.fillRect(101,75,107,165,BLACK);
  tft.fillRect(210,75,106,165,BLACK);
}

/*
void goodface(){
  tft.fillRect(3,36,97,190,BLACK);
  tft.drawGrayscaleBitmap(11,80,good,80,99);
}
void sosoface(){
  tft.fillRect(3,36,97,190,BLACK);
  tft.drawGrayscaleBitmap(11,80,soso,80,82);
}
void badface(){
  tft.fillRect(3,36,97,190,BLACK);
  tft.drawGrayscaleBitmap(11,80,bad,80,82);
}
void worstface(){
  tft.fillRect(3,36,97,190,BLACK);
  tft.drawGrayscaleBitmap(11,80,worst,80,70);
}*/


void printRPM(int cen_x, int cen_y, int width, float value, float vmin, float vmax, uint16_t color){
  //int rpm_width = width;
  //int rpm_x = cen_x;
  //int rpm_y = cen_y;
  int rpm_pin_length = (width/2)-10;
  //float rpm_value = value;
  //float rpm_min = vmin;
  //float rpm_max = vmax;
  float rpm_degree = 180 * (value/vmax);
  int rpm_pin_x = (int)(cen_x-rpm_pin_length*cos(rpm_degree*(PI/180)));
  int rpm_pin_y = (int)(cen_y-rpm_pin_length*sin(rpm_degree*(PI/180)));

  tft.drawCircle(cen_x, cen_y, width/2, color);
  tft.drawLine(cen_x,cen_y,rpm_pin_x,rpm_pin_y,color);
}

void TH(){
  
  while(MS.available())
  {
    strData = MS.readString();
    //Serial.println("string data = "+String(strData));

    idx_t = strData.indexOf('t');
    //Serial.print("index_t = "+String(idx_t)+"\t");

    if(idx_t != -1)
    {
      strTem = strData.substring(0,idx_t);  // 온도 저장
      //Serial.println("strTemL = "+String(strTem));
      strData = strData.substring(idx_t+1);
      //Serial.println("string data = "+String(strData));
    }
    
    idx_h = strData.indexOf('h');
    //Serial.print("idx_h = "+String(idx_h)+"\t");
    if(idx_h != -1)
    {
      strHum = strData.substring(0,idx_h);
      //Serial.println("strHum = "+String(strHum));
      strData = strData.substring(idx_h+1);
      //Serial.println("string data = "+String(strData));
    }

    //if((idx_t==-1)&&(idx_h==-1))
    //{
      //Serial.println("strData + "+String(strData));
      //Serial.println("Finished");
    //}

    //Serial.println("Tem: "+String(strTem));
    //Serial.println("Hum: "+String(strHum));
    float Temp = strTem.toFloat();
    float Humi = strHum.toFloat();
    float Humi2 = Humi/100;
    float fuck = (1.8 * Temp) - 0.55*(1-Humi2)*((1.8*Temp)-26) + 32;
    Serial.println(Temp);
    Serial.println(Humi);
    Serial.println(fuck);
    if(fuck>=80)
    {
      next = 4;
    }
    else if (fuck>=75)
    {
      next = 3;
    }
    else if(fuck>=68)
    {
      next = 2;
    }
    else
    {
      next = 1;
    }
    
    if (cur != next){
      cur = next;
      if (cur == 1){
        tft.fillRect(3,36,97,190,BLACK);
        bmpDraw("good.bmp" , 11, 85);
      }
      else if (cur == 2){
        tft.fillRect(3,36,97,190,BLACK);
        bmpDraw("soso.bmp" , 11, 85);
      }
      else if (cur == 3){
        tft.fillRect(3,36,97,190,BLACK);
        bmpDraw("bad.bmp" , 11, 85);
      }
      if (cur == 4){
        tft.fillRect(3,36,97,190,BLACK);
        bmpDraw("worst.bmp" , 11, 85);
      }
    }
    
    //Serial.println(Temp);
    //Serial.println(Humi);
    tft.setTextSize(3);
    clearMonitor();
    tft.setCursor(122,45);
    tft.println("TEMP");
    tft.setCursor(122,100);
    tft.print(strTem);
    tft.setCursor(230,45); 
    tft.println("HUMI");
    tft.setCursor(230,100);
    tft.println(strHum);
    Serial.println();
    printRPM(154,235,106,Temp,0,60,YELLOW);
    printRPM(262,235,106,Humi,0,100,GREEN);
    //goodface();
    
    
    break;

    
  }
}

void bmpDraw(char *filename, int x, int y) {

  File     bmpFile;
  int      bmpWidth, bmpHeight;   // W+H in pixels
  uint8_t  bmpDepth;              // Bit depth (currently must be 24)
  uint32_t bmpImageoffset;        // Start of image data in file
  uint32_t rowSize;               // Not always = bmpWidth; may have padding
  uint8_t  sdbuffer[3*BUFFPIXEL]; // pixel in buffer (R+G+B per pixel)
  uint16_t lcdbuffer[BUFFPIXEL];  // pixel out buffer (16-bit per pixel)
  uint8_t  buffidx = sizeof(sdbuffer); // Current position in sdbuffer
  boolean  goodBmp = false;       // Set to true on valid header parse
  boolean  flip    = true;        // BMP is stored bottom-to-top
  int      w, h, row, col;
  uint8_t  r, g, b;
  uint32_t pos = 0, startTime = millis();
  uint8_t  lcdidx = 0;
  boolean  first = true;

  if((x >= tft.width()) || (y >= tft.height())) return;

  Serial.println();
  progmemPrint(PSTR("Loading image '"));
  Serial.print(filename);
  Serial.println('\'');
  // Open requested file on SD card
  if ((bmpFile = SD.open(filename)) == NULL) {
    progmemPrintln(PSTR("File not found"));
    return;
  }

  // Parse BMP header
  if(read16(bmpFile) == 0x4D42) { // BMP signature
    progmemPrint(PSTR("File size: ")); Serial.println(read32(bmpFile));
    (void)read32(bmpFile); // Read & ignore creator bytes
    bmpImageoffset = read32(bmpFile); // Start of image data
    progmemPrint(PSTR("Image Offset: ")); Serial.println(bmpImageoffset, DEC);
    // Read DIB header
    progmemPrint(PSTR("Header size: ")); Serial.println(read32(bmpFile));
    bmpWidth  = read32(bmpFile);
    bmpHeight = read32(bmpFile);
    if(read16(bmpFile) == 1) { // # planes -- must be '1'
      bmpDepth = read16(bmpFile); // bits per pixel
      progmemPrint(PSTR("Bit Depth: ")); Serial.println(bmpDepth);
      if((bmpDepth == 24) && (read32(bmpFile) == 0)) { // 0 = uncompressed

        goodBmp = true; // Supported BMP format -- proceed!
        progmemPrint(PSTR("Image size: "));
        Serial.print(bmpWidth);
        Serial.print('x');
        Serial.println(bmpHeight);

        // BMP rows are padded (if needed) to 4-byte boundary
        rowSize = (bmpWidth * 3 + 3) & ~3;

        // If bmpHeight is negative, image is in top-down order.
        // This is not canon but has been observed in the wild.
        if(bmpHeight < 0) {
          bmpHeight = -bmpHeight;
          flip      = false;
        }

        // Crop area to be loaded
        w = bmpWidth;
        h = bmpHeight;
        if((x+w-1) >= tft.width())  w = tft.width()  - x;
        if((y+h-1) >= tft.height()) h = tft.height() - y;

        // Set TFT address window to clipped image bounds
        tft.setAddrWindow(x, y, x+w-1, y+h-1);

        for (row=0; row<h; row++) { // For each scanline...
          // Seek to start of scan line.  It might seem labor-
          // intensive to be doing this on every line, but this
          // method covers a lot of gritty details like cropping
          // and scanline padding.  Also, the seek only takes
          // place if the file position actually needs to change
          // (avoids a lot of cluster math in SD library).
          if(flip) // Bitmap is stored bottom-to-top order (normal BMP)
            pos = bmpImageoffset + (bmpHeight - 1 - row) * rowSize;
          else     // Bitmap is stored top-to-bottom
            pos = bmpImageoffset + row * rowSize;
          if(bmpFile.position() != pos) { // Need seek?
            bmpFile.seek(pos);
            buffidx = sizeof(sdbuffer); // Force buffer reload
          }

          for (col=0; col<w; col++) { // For each column...
            // Time to read more pixel data?
            if (buffidx >= sizeof(sdbuffer)) { // Indeed
              // Push LCD buffer to the display first
              if(lcdidx > 0) {
                tft.pushColors(lcdbuffer, lcdidx, first);
                lcdidx = 0;
                first  = false;
              }
              bmpFile.read(sdbuffer, sizeof(sdbuffer));
              buffidx = 0; // Set index to beginning
            }

            // Convert pixel from BMP to TFT format
            b = sdbuffer[buffidx++];
            g = sdbuffer[buffidx++];
            r = sdbuffer[buffidx++];
            lcdbuffer[lcdidx++] = tft.color565(r,g,b);
          } // end pixel
        } // end scanline
        // Write any remaining data to LCD
        if(lcdidx > 0) {
          tft.pushColors(lcdbuffer, lcdidx, first);
        } 
        progmemPrint(PSTR("Loaded in "));
        Serial.print(millis() - startTime);
        Serial.println(" ms");
      } // end goodBmp
    }
  }

  bmpFile.close();
  if(!goodBmp) progmemPrintln(PSTR("BMP format not recognized."));
}

// These read 16- and 32-bit types from the SD card file.
// BMP data is stored little-endian, Arduino is little-endian too.
// May need to reverse subscript order if porting elsewhere.

uint16_t read16(File f) {
  uint16_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read(); // MSB
  return result;
}

uint32_t read32(File f) {
  uint32_t result;
  ((uint8_t *)&result)[0] = f.read(); // LSB
  ((uint8_t *)&result)[1] = f.read();
  ((uint8_t *)&result)[2] = f.read();
  ((uint8_t *)&result)[3] = f.read(); // MSB
  return result;
}

// Copy string from flash to serial port
// Source string MUST be inside a PSTR() declaration!
void progmemPrint(const char *str) {
  char c;
  while(c = pgm_read_byte(str++)) Serial.print(c);
}

// Same as above, with trailing newline
void progmemPrintln(const char *str) {
  progmemPrint(str);
  Serial.println();
}
