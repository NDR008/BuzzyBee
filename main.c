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
    Image current_sprite;
    int frame_n;
    int total_frames;
    int y_pos;
    int x_pos;
    int x_vel;
    int y_vel;
} animatedObject;

animatedObject mainPlayer;

unsigned char *img_beepsrites[] = { img_beesprite0, img_beesprite1 };
//unsigned char *img_beepsrites[2] = { &img_beesprite0, &img_beesprite1 };

void initialize();
void startScreen();
void gameScreen();

void initialize() {
	initializeScreen();
	//PadInit(0);	
    in_init();

	setBackgroundColor(createColor(30, 30, 30));
	initializeDebugFont();

    //load sprites
}
int buffer = 0;

void updateAnimation(){
    if (input_trig & PAD_UP) {
        mainPlayer.y_vel -= 12;
        if (mainPlayer.y_vel < -20) {
            mainPlayer.y_vel = -20;
        }
    }

    mainPlayer.y_vel += 1;
    if (mainPlayer.y_vel > 5) {
        mainPlayer.y_vel = 5;
    }
    
    mainPlayer.y_pos += mainPlayer.y_vel;

    if (mainPlayer.y_pos > SCREEN_HEIGHT-20-mainPlayer.current_sprite.sprite.my){
        mainPlayer.y_pos = SCREEN_HEIGHT-20-mainPlayer.current_sprite.sprite.my;
    }
    else if (mainPlayer.y_pos < 20+mainPlayer.current_sprite.sprite.my){
        mainPlayer.y_pos = 20+mainPlayer.current_sprite.sprite.my;
    }

    if (mainTimer.vsync % 4 == 0) {
        mainPlayer.frame_n++;
        mainPlayer.current_sprite = createImage(img_beepsrites[mainPlayer.frame_n % (mainPlayer.total_frames+1)]);
    }

    mainPlayer.current_sprite = moveImage(mainPlayer.current_sprite, mainPlayer.x_pos, mainPlayer.y_pos);


}


void gameScreen(){
}

int main() {
    initialize();
    printf("BuzzyBee v0.1\n");
    mainPlayer.current_sprite = createImage(img_beepsrites[0]);
    mainPlayer.total_frames = 1;
    mainPlayer.y_pos = 50+mainPlayer.current_sprite.sprite.h;
    mainPlayer.x_pos = 150;
    mainPlayer.y_vel, mainPlayer.x_vel = 0;
    Box frame;
    frame = createBox(createColor(200, 155, 155), 20, 20, 320-20, 240-20);

    mainTimer = createTimer();
    while (1) {
        // printf("%i", mainTimer.vsync);
        clearDisplay();
        updateAnimation();
        mainPlayer.current_sprite = moveImage(mainPlayer.current_sprite, mainPlayer.x_pos, mainPlayer.y_pos);
        drawImage(mainPlayer.current_sprite);
        drawBox(frame);
        in_update();
        flushDisplay(); // dump it to the screen
        mainTimer = incTimer(mainTimer);
    }
}
