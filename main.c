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
#include <libspu.h>
#include "graphics/images.h"
#include "engine/graphics.h"
#include "engine/input.h"
#include "engine/timerz.h"
#include "engine/audio.h"
#include "sound/sfx/buzz1.h"

// Global system
#define OT_LENGTH 1
#define PACKETMAX 300
#define __ramsize   0x00200000
#define __stacksize 0x00004000

Image gameStartGraphic;

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
    int anim_rate; // how many vsyncs
} animatedObject;

animatedObject mainPlayer;

unsigned char *img_beepsrites[] = { img_bee_0, img_bee_1, img_bee_2, img_bee_1 };
//unsigned char *img_beepsrites[2] = { &img_beesprite0, &img_beesprite1 };

void initialize();
void startScreen();
void gameScreen();
void initPlayer();

void initialize() {
	initializeScreen();
    setBackgroundColor(createColor(30, 30, 30));
    audioInit();
    audioTransferVagToSPU(buzz1, buzz1_size, SPU_1CH);

    in_init();  // init inputs
    in_update(); // should not be needed but there is a bug without it
	//initializeDebugFont();
    initPlayer();
    //load sprites
}

#define factor 1024
#define GRAVITY factor / 8
#define MAXSPEED factor

#define MAXFLAP 5 * factor

#define SPRITEHEIGHT 30
#define GROUND (SCREEN_HEIGHT-20-30) * factor

#define CEILING (SPRITEHEIGHT)

int gameState = 0; // 0 = start state, 1 = play state, 2 = pause

void updateAnimation(){   
    // if we pressu jump...
    if (input_trig & PAD_CROSS) {
        audioPlay(SPU_1CH, 0x1000);
        mainPlayer.y_vel -= MAXFLAP;
        if (mainPlayer.y_vel < -MAXFLAP) {
            mainPlayer.y_vel = MAXFLAP;
        }
    }
    // constant desire to go down...
    mainPlayer.y_vel += GRAVITY;
    if (mainPlayer.y_vel > MAXSPEED) {
        mainPlayer.y_vel = MAXSPEED;
    }
    
    // update position
    mainPlayer.y_pos += mainPlayer.y_vel;

    // hover on the ground limit
    if (mainPlayer.y_pos > GROUND){
        mainPlayer.y_pos = GROUND;        
    }
    // stay within frame
    else if (mainPlayer.y_pos < CEILING){
        mainPlayer.y_pos = CEILING;
        mainPlayer.y_vel = 0;
    }

    // animate
    if (mainTimer.vsync % mainPlayer.anim_rate == 0) {
        mainPlayer.frame_n++;
        mainPlayer.current_sprite = createImage(img_beepsrites[mainPlayer.frame_n % (mainPlayer.total_frames)]);
    }
    mainPlayer.current_sprite = moveImage(mainPlayer.current_sprite, mainPlayer.x_pos / factor, mainPlayer.y_pos /factor);
    drawImage(mainPlayer.current_sprite);
}

void gameScreen(){
    gameStartGraphic = createImage(img_a_BuzzyBee);
    gameStartGraphic = moveImage(gameStartGraphic, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);
    drawImage(gameStartGraphic);
}

void initPlayer(){
    mainPlayer.current_sprite = createImage(img_beepsrites[0]);
    mainPlayer.total_frames = 4;
    mainPlayer.y_pos = GROUND * factor;
    mainPlayer.x_pos = (SCREEN_WIDTH / 4) * factor;
    mainPlayer.y_vel, mainPlayer.x_vel = 0;
    mainPlayer.anim_rate = 4;
}

int main() {
    initialize();
    printf("BuzzyBee v0.11\n");

    Box frame;
    frame = createBox(createColor(200, 155, 155), 20, 20, 320-20, 240-20);
    mainTimer = createTimer();

    while (1) {
        in_update();
        clearDisplay();
        if (gameState == 1){
            updateAnimation();
                    
        }
        else if (gameState == 0){
            gameScreen();
            if (input_trig & PAD_START) {
                audioPlay(SPU_1CH, 0x1000);
                gameState = 1;
                }
        }

        //mainPlayer.current_sprite = moveImage(mainPlayer.current_sprite, mainPlayer.x_pos/, mainPlayer.y_pos);
        
        drawBox(frame);
        flushDisplay(); // dump it to the screen
        mainTimer = incTimer(mainTimer);
    }
}
