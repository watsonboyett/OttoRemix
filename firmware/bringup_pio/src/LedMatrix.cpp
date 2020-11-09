#include "LedMatrix.h"
#include "LedBitmaps.h"
#include <SPI.h> // PIO needs this?
#include <Adafruit_LEDBackpack.h>
#include <Adafruit_GFX.h>

const int LEDM_ADDR = 0x70;

Adafruit_8x16minimatrix matrix = Adafruit_8x16minimatrix();


void ledm_init()
{
  matrix.begin(LEDM_ADDR);
  matrix.setBrightness(16);
}

void ledm_update()
{
  static int ledm_count = 0;

  matrix.clear();

  //matrix.drawPixel(ledm_count % 8, ledm_count % 16, LED_ON);

  const uint8_t * face = 0;
  switch (ledm_count % 4)
  {
    case 0:
      {
        face = smile_bmp;
        break;
      }
    case 1:
      {
        face = neutral_bmp;
        break;
      }
    case 2:
      {
        face = frown_bmp;
        break;
      }
    case 3:
      {
        face = mute_bmp;
        break;
      }
  }

  matrix.drawBitmap(0, 0, face, 8, 16, LED_ON);
  matrix.writeDisplay();

  ledm_count++;
}
