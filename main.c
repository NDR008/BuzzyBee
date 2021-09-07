// main.c BuzzyBee v0.1 for 32bitJam 2021

/*

MIT License

Copyright (c) 2021 NDR008

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

// #include "common/syscalls/syscalls.h"
#include <sys/types.h>
#include <libetc.h>
//#include <libpad.h>
//#include <hardware/pcsxhw.h>
#include <stdlib.h>
#include <stdio.h>
#include <libgte.h>
#include <libgpu.h>
#include <libgs.h>
#include "libs/images.h"
#include "engine/basics.h"
#include "engine/input.h"

// Global system
#define OT_LENGTH 1
#define PACKETMAX 300
#define __ramsize   0x00200000
#define __stacksize 0x00004000

// temp sprites
Image ballSprite;
Image playerSprite;
Image bgSprite;

// functions/routines in this file

void initialize();
void startScreen();
void gameScreen();

void initialize() {
	initializeScreen();
	//PadInit(0);	
    in_init();

	setBackgroundColor(createColor(30, 30, 30));
	initializeDebugFont();

    //init scene
	ballSprite = createImage(img_ball);
	ballSprite = scaleImage(ballSprite, 50, 50);
	playerSprite = createImage(img_playerBar);
	bgSprite = createImage(img_wall1);
	bgSprite = scaleImage(bgSprite, 200, 200);
	bgSprite = moveImage(bgSprite, 160, 120);
}

void gameScreen(){

}

int main() {
    initialize();
    printf("BuzzyBee\n");
    while (1) {
        clearDisplay();
        in_update();
        flushDisplay(); // dump it to the screen
    }
}
