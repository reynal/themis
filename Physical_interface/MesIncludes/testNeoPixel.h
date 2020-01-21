#ifndef TESTNEOPIXEL
#define TESTNEOPIXEL

#include "neoPixel.h"

//colors
#define BLANC  0x00FFFFFF
#define PALE   0x00333333
#define ROUGE  0x00FF0000
#define BLEU   0x000000FF
#define VERT   0x0000FF00
#define VIOLET 0x00FF00FF

void tnP_testGPIO();

void tnP_testSPI();
void tnP_testLed0();
void tnP_test1();
void tnP_test2();

#endif
