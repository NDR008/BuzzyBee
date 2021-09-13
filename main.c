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
#include "engine/timerz.h"

// Global system
#define OT_LENGTH 1
#define PACKETMAX 300
#define __ramsize   0x00200000
#define __stacksize 0x00004000

// temp sprites
Image playerSprite;

// Global timer
PSXTimer mainTimer;

typedef struct{
    Image sprite;
    int frame_n;
    int total_frames;
    int y_pos;
    int x_pos;
} animatedObject;

animatedObject mainPlayer;
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
}

void updateAnimation(){
    if (input_trig & PAD_UP) {
        mainPlayer.y_pos -= 10;
        mainPlayer.sprite = moveImage(mainPlayer.sprite, mainPlayer.x_pos, mainPlayer.y_pos);
        return;
    }
    if (mainTimer.vsync % 10 == 0) {
        mainPlayer.frame_n++;
        if (mainPlayer.frame_n > mainPlayer.total_frames){
            mainPlayer.frame_n = 0;
            mainPlayer.sprite = createImage(img_beesprite0);
        }
        else {
            mainPlayer.sprite = createImage(img_beesprite1);
        }
    }
    else if (mainTimer.vsync % 5 == 0) {
            mainPlayer.y_pos += 5;
            if (mainPlayer.y_pos > 50){
                mainPlayer.y_pos = 50;
            }
    }
}


void gameScreen(){
}

int main() {
    initialize();
    printf("BuzzyBee\n");
    mainPlayer.sprite = createImage(img_beesprite0);
    mainPlayer.total_frames = 1;
    mainPlayer.y_pos = 0;
    mainPlayer.x_pos = 0;
    

    mainTimer = createTimer();
    while (1) {
        // printf("%i", mainTimer.vsync);
        clearDisplay();
        updateAnimation();
        mainPlayer.sprite = moveImage(mainPlayer.sprite, mainPlayer.x_pos, mainPlayer.y_pos);
        drawImage(mainPlayer.sprite);
        in_update();
        flushDisplay(); // dump it to the screen
        mainTimer = incTimer(mainTimer);
    }
}
