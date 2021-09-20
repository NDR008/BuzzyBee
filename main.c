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
#include "sound/sfx/menustart.h"

// Global timer
PSXTimer mainTimer;

typedef struct{
    int frame_n;            // this is the current frame
    int index;
    int total_frames;       // this is the total frame
    int y_pos;
    int x_pos;
    int x_vel;
    int y_vel;
    int anim_rate; // how many vsyncs
    Image *img_list;   // make this a list of Image (ok)
} AnimatedObject;

AnimatedObject mainPlayer;
Image mainPlayerL[4];

AnimatedObject pressStart;
Image pressStartL[3];

AnimatedObject Bee;
Image BeeL[1];

AnimatedObject Buzzy;
Image BuzzyL[1];

AnimatedObject jam;
Image jamL[3];

AnimatedObject ground[6];
Image groundL[6][1];


AnimatedObject clouds1[3];
Image clouds1L[3][1];
AnimatedObject clouds2[3];
Image clouds2L[3][1];

void initialize();
void startScreen();
void gameStart();
void gameMode();
void debug14Mode();
void initPlayer();
void initIntro();
void debugMode();
void animate(AnimatedObject *animatedObj);
void initBackground();
void initSky();

void initialize() {
	initializeScreen();
    setBackgroundColor(createColor(0, 0, 0));
    audioInit();
    audioTransferVagToSPU(buzz1, buzz1_size, SPU_1CH);
    audioTransferVagToSPU(menustart, menustart_size, SPU_0CH);

    in_init();  // init inputs
    in_update(); // should not be needed but there is a bug without it
	initializeDebugFont();
    initPlayer();
    initBackground();
    initSky();
    initIntro();
    //load sprites
}

// scaling the graphics (example video width is 320px but "game world" is 320 x factor)
#define factor 1024     
#define GRAVITY factor / 4
#define MAXSPEED factor
#define MAXFLAP 6 * factor
#define SPRITEHEIGHT 30
#define GROUND (SCREEN_HEIGHT-80) * factor
#define CEILING (SPRITEHEIGHT)
int a, b, c = 0; // a is for bringing in Buzzy, b for bringing in Bee, c from bringing in Jam

int gameState = 0; // 0 = start state, 1 = play state, 2 = pause

void gameMode(){   
    setBackgroundColor(createColor(100, 100, 200));
    // if we pressu jump...
    if (input_trig & PAD_CROSS) {
        audioPlay(SPU_1CH, 0x1000);
        mainPlayer.y_vel -= MAXFLAP/2;
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

    // this blog corrupts shit
    
    for (int i = 0; i < 3; i++){
        clouds1[i].x_pos += clouds1[i].x_vel;
        clouds2[i].x_pos += clouds2[i].x_vel;
        if (clouds1[i].x_pos < - 50 * factor){
            clouds1[i].x_pos = (400 + rand() % 50) *factor;
        }
        if (clouds2[i].x_pos < - 100 * factor){
            clouds2[i].x_pos = (450 + rand() % 50) *factor;
        }
        animate(&clouds1[i]);
        animate(&clouds2[i]);
    }
    

    
    for (int i=0; i<6; i++){
        ground[i].x_pos += ground[i].x_vel;
        if (ground[i].x_pos + 40 * factor <= 0){
            ground[i].x_pos = (80*6-41)*factor;
        }
        animate(&ground[i]);
    }

    animate(&mainPlayer);
}

void gameStart(){
    setBackgroundColor(createColor(0, 0, 0));
    int Buzzy_limit = (BuzzyL[0].sprite.w / 2) * factor;
    if (Buzzy.x_pos < Buzzy_limit) {
        Buzzy.x_pos += Buzzy.x_vel;
    }
    else {    
        a = 1;
    }

    int Bee_limit = (SCREEN_WIDTH/2 + BeeL[0].sprite.w / 2 + 10) * factor;
    if (Bee.x_pos > Bee_limit) {
        Bee.x_pos += Bee.x_vel;    
    }
    else {
        b = 1;
    }
    if (a + b == 2) {
        int jam_limit = 150 * factor;
        if (jam.y_pos > jam_limit) {
            jam.y_pos += jam.y_vel;    
        }
        else {
            c = 1;
        }
        animate(&jam);
    }

    if (c) {
        animate(&pressStart);
    }
    animate(&Bee);
    animate(&Buzzy);
}

void animate(AnimatedObject *animatedObj){
    Image toDraw;
    if (mainTimer.vsync % animatedObj->anim_rate == 0) {
        animatedObj->frame_n++;
        animatedObj->index = animatedObj->frame_n % animatedObj->total_frames;
    }
    toDraw = moveImage(animatedObj->img_list[animatedObj->index], animatedObj->x_pos / factor, animatedObj->y_pos / factor);
    drawImage(toDraw);
}

void debugMode(){
    setBackgroundColor(createColor(50, 50, 0));
    Image debugJam = createImage(img_jam_1);
    debugJam = moveImage(debugJam, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4);    
    Image debugBee = createImage(img_bee_0);    
    debugBee = moveImage(debugBee, SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2);    
    
    drawImage(debugJam);
    drawImage(debugBee);
}

void initPlayer(){
    mainPlayer.total_frames = 4;
    mainPlayer.frame_n = 0;
    mainPlayer.index = 0;
    mainPlayer.y_pos = (SCREEN_HEIGHT * 3 / 4) * factor;
    mainPlayer.x_pos = (SCREEN_WIDTH / 4) * factor;
    mainPlayer.y_vel, mainPlayer.x_vel = 0;
    mainPlayer.anim_rate = 4;
    mainPlayerL[0] = createImage(img_bee_0);
    mainPlayerL[1] = createImage(img_bee_1);
    mainPlayerL[2] = createImage(img_bee_2);
    mainPlayerL[3] = mainPlayerL[1];
    mainPlayer.img_list = mainPlayerL;
}

void initBackground(){
    for (int s=0; s<6; s++){
        AnimatedObject tmp;
        ground[s].total_frames = 1;
        ground[s].frame_n = 0;
        ground[s].index = 0;
        ground[s].y_pos = (SCREEN_HEIGHT - 30) * factor;
        ground[s].x_pos = (s*80-40) * factor;
        ground[s].y_vel = 0;
        ground[s].x_vel = -1 * factor;
        ground[s].anim_rate = 99;
        groundL[s][0] = createImage(img_gnd_0);
        ground[s].img_list = groundL[s];
    }
}

void initSky(){
    srand(1);
    for (int s=0; s<3; s++){
        int x1 = rand() % (100) * factor;
        int y1 = (10+ rand() % 20) * factor;
        int s1 = (1 + rand() % 2) * factor;

        int x2 = rand() % (100) * factor;
        int y2 = (10+ rand() % 20) * factor;
        int s2 = (1 + rand() % 2) * factor;

        clouds1[s].total_frames =1;
        clouds2[s].total_frames = 1;
        clouds1[s].frame_n = 0;
        clouds2[s].frame_n = 0;
        clouds1[s].index = 0;
        clouds2[s].index = 0;
        clouds1[s].y_vel = 0;
        clouds2[s].y_vel = 0;

        clouds1[s].y_pos = y1;
        clouds2[s].y_pos = y2;
        clouds1[s].x_pos = x1;
        clouds2[s].x_pos = x2;
        clouds1[s].x_vel = -s1;
        clouds2[s].x_vel = -s2;

        clouds1[s].anim_rate = 99;
        clouds2[s].anim_rate = 99;

        clouds1L[s][0] = createImage(img_cloud_B);
        clouds2L[s][0] = createImage(img_cloud_E);

        clouds1[s].img_list = clouds1L[s];
        clouds2[s].img_list = clouds2L[s];
    }
}

void initIntro(){
    pressStart.total_frames = 2; // third frame is reserved
    pressStart.frame_n = 0;
    pressStart.index = 0;
    pressStart.y_pos = (SCREEN_HEIGHT * 3 / 4) * factor;
    pressStart.x_pos = (SCREEN_WIDTH / 2) * factor;
    pressStart.y_vel, pressStart.x_vel = 0;
    pressStart.anim_rate = 25;
    pressStartL[0] = createImage(img_Press_start1);
    pressStartL[1] = createImage(img_Press_start2);
    pressStartL[2] = createImage(img_Press_start3);
    pressStart.img_list = pressStartL;


    Buzzy.total_frames = 1; 
    Buzzy.frame_n = 0;
    Buzzy.index = 0;
    Buzzy.x_pos = (5) * factor;
    Buzzy.y_vel = 0;
    Buzzy.x_vel = 3 * factor;
    Buzzy.anim_rate = 1;
    BuzzyL[0] = createImage(img_Buzzy);
    Buzzy.y_pos = (BuzzyL[0].sprite.h / 3) * factor;
    Buzzy.x_pos = (-BuzzyL[0].sprite.w/2) * factor;
    Buzzy.img_list = BuzzyL;

    Bee.total_frames = 1; // third frame is reserved
    Bee.frame_n = 0;
    Bee.index = 0;
    Bee.y_vel = 0;
    Bee.x_vel = -2 * factor;
    Bee.anim_rate = 1;
    BeeL[0] = createImage(img_Bee);
    Bee.y_pos = (BeeL[0].sprite.h) * factor;
    Bee.x_pos = (SCREEN_WIDTH + BeeL[0].sprite.w/2) * factor;
    Bee.img_list = BeeL;

    jam.total_frames = 3; // third frame is reserved
    jam.frame_n = 0;
    jam.index = 0;
    jam.y_vel = -1 * factor;
    jam.x_vel = 0;
    jam.anim_rate = 10;
    jamL[0] = createImage(img_jam_1);
    jamL[1] = createImage(img_jam_2);
    jamL[2] = createImage(img_jam_3);
    jam.y_pos = SCREEN_HEIGHT * factor;
    jam.x_pos = (SCREEN_WIDTH / 2) * factor;
    jam.img_list = jamL;
}

int main() {
  initialize();
    printf("BuzzyBee v0.13 New Animation routine\n");
    mainTimer = createTimer();
    int time1;
    while (1) {
        int time2 = mainTimer.totalsec;
        in_update();
        clearDisplay();
        if (gameState == 1){
            if (time2-time1 > 0){  // check back to 1;
                gameMode();
            }
            else{
                gameStart();
            }
        }
        else if (gameState == 0){
            gameStart();
        }
        else if (gameState == 99){
            debugMode();
        }

        if (input_trig & PAD_START) {
            audioPlay(SPU_0CH, 0x1000);
            time1 = mainTimer.totalsec;
            pressStart.total_frames = 3;
            pressStart.anim_rate = 5;
            gameState = 1;
        }
        if (input_trig & PAD_SELECT) {
            gameState = 99;
        }

        //printf("Game mode %i\n", gameState);

        flushDisplay(); // dump it to the screen
        mainTimer = incTimer(mainTimer);
    }
}
