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
#include "sound/sfx/beeHurt.h"
#include "sound/sfx/beePU.h"

// Global timer
PSXTimer mainTimer;

int tracks[] = {2, 0}; 

typedef struct AnimatedObject{
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

typedef struct Health {
    int level;
    Image *bar;
    Image art;
} Health;
Image barL[5];

AnimatedObject mainPlayer;
Image mainPlayerL[4];
Health mainHealth;

# define enemiesN 1
AnimatedObject enemies;
Image enemiesL[4];

AnimatedObject pressStart;
Image pressStartL[3];

AnimatedObject Bee;
Image BeeL[1];

AnimatedObject Buzzy;
Image BuzzyL[1];

AnimatedObject jam;
Image jamL[3];

//const int groundN = 10;
#define groundN 11
AnimatedObject ground[groundN];
Image groundL[groundN][1];

#define rocksN 3
AnimatedObject rocks[rocksN];
Image rocksL[rocksN][1];

AnimatedObject flowers[rocksN];
Image flowersL[rocksN][1];

#define hillsN 5
AnimatedObject hills[hillsN];
Image hillsL[hillsN][1];

# define cloudsN 2
AnimatedObject clouds1[cloudsN];
Image clouds1L[cloudsN][1];
AnimatedObject clouds2[cloudsN];
Image clouds2L[cloudsN][1];

int hitE = 0;
int hitF = 0;
int score = 0;
int bestScore = 0;
int time3; // when player won

void initialize();
void startScreen();
void gameIntro();
void gameMode();
void debug14Mode();
void initPlayer();
void initIntro();
void debugMode();
void animate(AnimatedObject *animatedObj);
void initGround();
void initSky();
void initRocks();
int checkHit(AnimatedObject *animatedObj, int pad);
void initEnemies();
void initHills();
void gameOver();

void initialize() {
    srand(1);
	initializeScreen();
    initializeDebugFont();
    setBackgroundColor(createColor(0, 0, 0));
    audioInit();
    audioTransferVagToSPU(buzz1, buzz1_size, SPU_1CH);
    audioTransferVagToSPU(menustart, menustart_size, SPU_0CH);
    audioTransferVagToSPU(beeHurt, beeHurt_size, SPU_2CH);
    audioTransferVagToSPU(beePU, beePU_size, SPU_3CH);

    in_init();  // init inputs
    //in_update(); // should not be needed but there is a bug without it
	initializeDebugFont();
    initIntro();

    initPlayer();
    initEnemies();
    initGround();
    initSky();
    initRocks();
    initHills();

    DsPlay(2, tracks, 0);
}

// scaling the graphics (example video width is 320px but "game world" is 320 x factor)
#define factor 1024     
#define GRAVITY factor / 4
#define MAXSPEED factor
#define MAXFLAP 6 * factor
#define SPRITEHEIGHT 18
#define GROUND (SCREEN_HEIGHT-80 - SPRITEHEIGHT) * factor
#define CEILING (SPRITEHEIGHT) * factor
const int mPlayer_x1 = 15 ;
const int mPlayer_x2 = mPlayer_x1 + 22;

int a, b, c = 0; // a is for bringing in Buzzy, b for bringing in Bee, c from bringing in Jam

int gameState = 0; // 0 = start state, 1 = play state, 2 = pause

// modified for 0,0 origin
void gameIntro(){
    setBackgroundColor(createColor(0, 0, 0));
    const int Buzzy_limit = (20) * factor;
    if (Buzzy.x_pos < Buzzy_limit) {
        Buzzy.x_pos += Buzzy.x_vel;
    }
    else {    
        a = 1;
    }

    const int Bee_limit = (SCREEN_WIDTH - 20 - Bee.img_list[0].sprite.w) * factor;
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

// need to fix
void gameMode(){   
    setBackgroundColor(createColor(100, 100, 200));
    //FntPrint("Health %d, HitF %d, HitE %d", mainHealth.level, hitF, hitE);

    int drawHealth = (int) mainHealth.level / 1000;
    for (int i=0; i<drawHealth; i++){
        drawImage(mainHealth.bar[i]);
    }
    
    mainHealth.level -= 3;

    // enemies on top
    enemies.x_pos += enemies.x_vel;
    if (enemies.x_pos < (-20 * factor)){
        enemies.x_pos = (SCREEN_WIDTH + 50) * factor;
        enemies.y_pos = 5;
    }
    enemies.y_pos += enemies.y_vel;
    if (enemies.y_pos < (2 * factor)){
        enemies.y_vel = (1 + rand()%3) * factor;
    }
    else if (enemies.y_pos > GROUND){
        enemies.y_vel = (-1 - rand()%3 ) * factor;
    }
    animate(&enemies);
    int oldHitE = hitE;
    hitE = checkHit(&enemies, 5);
    if (hitE && !oldHitE) {
        audioPlay(SPU_2CH, 0x1000, 0x1000);
        mainHealth.level -= 1500;
    }

    // if we press X to jump...
    if (input_trig & PAD_CROSS) {
        audioPlay(SPU_1CH, 0x0500, 0x1000);
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
    if (mainPlayer.y_pos > GROUND + 30 * factor){
        mainPlayer.y_pos = GROUND + 30 * factor;        
    }
    // stay within frame
    else if (mainPlayer.y_pos < CEILING){
        mainPlayer.y_pos = CEILING;
        mainPlayer.y_vel = 0;
    }

    for (int i = 0; i < cloudsN; i++){
        clouds1[i].x_pos += clouds1[i].x_vel;
        clouds2[i].x_pos += clouds2[i].x_vel;
        if (clouds1[i].x_pos < - 50 * factor){
            clouds1[i].x_pos = (400 + rand() % 150) *factor;
        }
        if (clouds2[i].x_pos < - 100 * factor){
            clouds2[i].x_pos = (450 + rand() % 350) *factor;
        }
        animate(&clouds2[i]);
        animate(&clouds1[i]);
    }

    animate(&mainPlayer);

    // fixed
    for (int i=0; i < rocksN; i++){
        int rock_wdith = rocks[0].img_list[0].sprite.w * factor / 2;
        int flower_height = flowers[0].img_list[0].sprite.h * factor;
        rocks[i].x_pos += rocks[i].x_vel;
        flowers[i].x_pos += rocks[i].x_vel;
        if (rocks[i].x_pos + 80 * factor <= 0){
            int x1 = (SCREEN_WIDTH + 100 + rand() % 50) * factor;
            int y1 = (SCREEN_HEIGHT - 80 - rand() % 120) * factor;
            rocks[i].y_pos = y1;
            rocks[i].x_pos = x1;
            flowers[i].x_pos = rocks[i].x_pos + rock_wdith;
            flowers[i].y_pos = rocks[i].y_pos - flower_height;
        }
        animate(&rocks[i]);
        animate(&flowers[i]);

        int oldHitF = hitF;
        hitF = checkHit(&flowers[i], 1);
        if (hitF && !oldHitF) {
            audioPlay(SPU_3CH, 0x1000, 0x1000);
            mainHealth.level += 1000;
            flowers[i].x_pos = -50 * factor;
        }
    }    
    
    const int ground_width = ground[0].img_list[0].sprite.w;
    for (int i=0; i<groundN; i++){
        ground[i].x_pos += ground[i].x_vel;
        if (ground[i].x_pos + ground_width * factor <= 0){
            ground[i].x_pos = (SCREEN_WIDTH)*factor;
        }
        animate(&ground[i]);
    }

    const int hills_width = hills[0].img_list[0].sprite.w;
    for (int i=0; i<hillsN; i++){
        hills[i].x_pos += hills[i].x_vel;
        if (hills[i].x_pos + hills_width * factor <= 0){
            hills[i].x_pos = (SCREEN_WIDTH)*factor;
        }
        animate(&hills[i]);
    }
    score++;
    if (mainHealth.level > 5999) {mainHealth.level = 5999;}
    if (mainHealth.level < 0) {
        mainHealth.level = 0;
        if (score > bestScore) { bestScore = score;}
        pressStart.total_frames = 2;
        gameState = 5;
        time3 = mainTimer.totalsec;
    }
}

int checkHit(AnimatedObject *animatedObj, int pad){
    // AABB detection
    int new_hit = 0;
    int target_y1 = animatedObj->y_pos / factor; // target top left
    int target_y2 = target_y1 + animatedObj->img_list[0].sprite.h; // target bottom left
    int target_x1 = animatedObj->x_pos / factor;
    int target_x2 = target_x1 + animatedObj->img_list[0].sprite.w;
    int mPlayer_y1 = mainPlayer.y_pos / factor;
    int mPlayer_y2 = mPlayer_y1 + mainPlayer.img_list[0].sprite.h;
    //printf("mPlayer_y1, %i, target_y2, %i\n", mPlayer_y1, target_y2);

    return mPlayer_x2 >= target_x1 + pad
        && mPlayer_x1 <= target_x2 - pad
        && mPlayer_y2 >= target_y1 + pad
        && mPlayer_y1 <= target_y2 - pad;
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

void gameOver(){
    setBackgroundColor(createColor(15, 15, 15)); // why not?
    FntPrint("Your Last attempt: %d\n", score);
    FntPrint("Your Best score: %d\n", bestScore);
    FntPrint("\nCredits to: \n- NDR008 (Producer/Dev), \n- Chadow (art),\n- nooisy -aka muda (audio), \n- KEROBOY (art)\n- ABelliqueux (3D Logo)");
    FntPrint("\n\nThanks to: \nNicolas, Sickle, Arthur,\nfgsfds, peach, \nSquaresoft74, Danhans42,\nYetAnotherEmuDev, crimester");
}

void debugMode(){
    setBackgroundColor(createColor(15, 15, 15)); // why not?
    Image debugJam = createImage(img_jam_1);
    debugJam = moveImage(debugJam, SCREEN_WIDTH / 2 - debugJam.sprite.w/2, SCREEN_HEIGHT / 4);    
    Image debugBee = createImage(img_bee_0);    
    debugBee = moveImage(debugBee, SCREEN_WIDTH / 2  - debugBee.sprite.h/2, SCREEN_HEIGHT / 2);    
    
    drawImage(debugJam);
    drawImage(debugBee);
}

// fixed
void initPlayer(){
    mainHealth.level = 5999;
    for (int i=0; i<5; i++){
        barL[i] = moveImage(scaleImage(createImage(img_health),400,40),20+i*20, 15);
    }
    mainHealth.bar = barL;

    mainPlayer.total_frames = 4;
    mainPlayer.frame_n = 0;
    mainPlayer.index = 0;
    mainPlayer.y_pos = GROUND + 30 * factor;
    mainPlayer.x_pos = mPlayer_x1 * factor;
    mainPlayer.y_vel = 0;
    mainPlayer.x_vel = 0;
    mainPlayer.anim_rate = 4;
    mainPlayerL[0] = createImage(img_bee_0);
    mainPlayerL[1] = createImage(img_bee_1);
    mainPlayerL[2] = createImage(img_bee_2);
    mainPlayerL[3] = mainPlayerL[1];
    mainPlayer.img_list = mainPlayerL;
}

void initEnemies(){
    enemies.total_frames = 4;
    enemies.frame_n = 0;
    enemies.index = 0;
    enemies.y_pos = 30;
    enemies.x_pos = (SCREEN_WIDTH + 10) * factor;
    enemies.y_vel = 2 * factor;
    enemies.x_vel = -3 * factor / 2;
    enemies.anim_rate = 4;
    enemiesL[0] = createImage(img_wasp1);
    enemiesL[1] = createImage(img_wasp2);
    enemiesL[2] = createImage(img_wasp3);
    enemiesL[3] = enemiesL[1];
    enemies.img_list = enemiesL;
}


// fixed
void initRocks(){
    for (int s=0; s< rocksN; s++){
        rocks[s].total_frames = 1;
        rocks[s].frame_n = 0;
        rocks[s].x_vel = -1 * factor;
        rocks[s].index = 0;
        rocks[s].anim_rate = 99;
        rocks[s].x_pos = (SCREEN_WIDTH * s + 50 + rand() % 100) * factor;
        rocks[s].y_pos = (SCREEN_HEIGHT - 120 - rand() % 100) * factor;
        rocksL[s][0] = createImage(img_gnd_1);
        rocks[s].img_list = rocksL[s];        

        flowers[s].total_frames = rocks[s].total_frames;
        flowers[s].frame_n = rocks[s].frame_n;
        flowers[s].x_vel = rocks[s].x_vel;
        flowers[s].anim_rate = 99;
        flowersL[s][0] = createImage(img_flower3);
        flowers[s].img_list = flowersL[s];       
        flowers[s].x_pos = rocks[s].x_pos + rocks[0].img_list[0].sprite.w * factor / 2;
        flowers[s].y_pos = rocks[s].y_pos - flowers[0].img_list[0].sprite.h * factor;
    }
}

void initHills(){
    for (int ind=0; ind<hillsN; ind++){
        hills[ind].total_frames = 1;
        hills[ind].frame_n = 0;
        hills[ind].index = 0;
        hills[ind].y_vel = 0;
        hills[ind].x_vel = -1 * factor / 4;
        hills[ind].anim_rate = 99;
        //int select_img = rand() % 5;
        int select_img = 4;
        if (select_img == 4){
            hillsL[ind][0] = createImage(img_background1layer2);
        }
        else {
            hillsL[ind][0] = createImage(img_background1layer1);
        }
        hills[ind].img_list = hillsL[ind];
        hills[ind].y_pos = (SCREEN_HEIGHT - hills[0].img_list[0].sprite.h + 10) * factor;
        hills[ind].x_pos = (ind*hills[0].img_list[0].sprite.w) * factor;
    }
}

//fixed
void initGround(){
    for (int ind=0; ind<groundN; ind++){
        ground[ind].total_frames = 1;
        ground[ind].frame_n = 0;
        ground[ind].index = 0;
        ground[ind].y_vel = 0;
        ground[ind].x_vel = -1 * factor;
        ground[ind].anim_rate = 99;
        groundL[ind][0] = createImage(img_gnd_0);
        ground[ind].img_list = groundL[ind];
        ground[ind].y_pos = (SCREEN_HEIGHT - ground[0].img_list[0].sprite.h) * factor;
        ground[ind].x_pos = (ind*ground[0].img_list[0].sprite.w) * factor;
    }
}

void initSky(){
    for (int s=0; s<cloudsN; s++){
        int x1 = rand() % (500) * factor;
        int y1 = (50+ rand() % 24) * factor;
        int s1 = (1 + rand() % 2) * factor;

        int x2 = rand() % (500) * factor;
        int y2 = (20+ rand() % 24) * factor;
        int s2 = (2 + rand() % 3) * factor;

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

        clouds1L[s][0] = scaleImage(createImage(img_cloud), 50, 50);
        clouds2L[s][0] = createImage(img_cloud);

        clouds1[s].img_list = clouds1L[s];
        clouds2[s].img_list = clouds2L[s];
    }
}

void initIntro(){
    Buzzy.total_frames = 1; 
    Buzzy.frame_n = 0;
    Buzzy.index = 0;
    Buzzy.x_pos = (5) * factor;
    Buzzy.y_vel = 0;
    Buzzy.x_vel = 3 * factor;
    Buzzy.anim_rate = 1;
    BuzzyL[0] = createImage(img_Buzzy);
    Buzzy.img_list = BuzzyL;
    Buzzy.y_pos = (0) * factor;
    Buzzy.x_pos = (-BuzzyL[0].sprite.w - 1) * factor;

    Bee.total_frames = 1; // third frame is reserved
    Bee.frame_n = 0;
    Bee.index = 0;
    Bee.y_vel = 0;
    Bee.x_vel = -2 * factor;
    Bee.anim_rate = 1;
    BeeL[0] = createImage(img_Bee);
    Bee.img_list = BeeL;
    Bee.y_pos = (60) * factor;
    Bee.x_pos = (SCREEN_WIDTH + 1) * factor;

    jam.total_frames = 3; // third frame is reserved
    jam.frame_n = 0;
    jam.index = 0;
    jam.y_vel = -1 * factor;
    jam.x_vel = 0;
    jam.anim_rate = 10;
    jamL[0] = createImage(img_jam_1);
    jamL[1] = createImage(img_jam_2);
    jamL[2] = createImage(img_jam_3);
    jam.img_list = jamL;
    jam.y_pos = (SCREEN_HEIGHT - 45) * factor;
    jam.x_pos = (SCREEN_WIDTH / 2 - jam.img_list[0].sprite.w/2) * factor;

    pressStart.total_frames = 2; // third frame is reserved
    pressStart.frame_n = 0;
    pressStart.index = 0;
    pressStart.y_vel = 0;
    pressStart.x_vel = 0;
    pressStart.anim_rate = 15;
    pressStartL[0] = createImage(img_Press_start1);
    pressStartL[1] = createImage(img_Press_start2);
    pressStartL[2] = createImage(img_Press_start3);
    pressStart.img_list = pressStartL;
    pressStart.y_pos = (SCREEN_HEIGHT - 55) * factor;
    pressStart.x_pos = (SCREEN_WIDTH / 2 - pressStart.img_list[0].sprite.w/2) * factor;

}

int main() {
    initialize();
    printf("BuzzyBee v1.0 (Release) \n");
    mainTimer = createTimer();
    int time1;
    while (1) {
        int time2 = mainTimer.totalsec;
        in_update();
        clearDisplay();
        if (gameState == 1){
            if (time2-time1 > 1){  // check back to 1;
                gameMode();
            }
            else{
                gameIntro();
            }
        }
        else if (gameState == 0){
            gameIntro();
        }
        else if (gameState == 99){
            debugMode();
        }
        else if (gameState == 5){
            gameOver();
            if (time2 - time3 > 6){
                score = 0;
                gameState = 0;
                initPlayer();
                initEnemies();
                initGround();
                initSky();
                initRocks();
                initHills();
            }
        }
        if (input_trig & PAD_START) {
            if (gameState == 0 & (time2 >= 2)){
                time1 = mainTimer.totalsec;
                audioPlay(SPU_0CH, 0x1000, 0x1000);
                pressStart.total_frames = 3;
                pressStart.anim_rate = 5;
                gameState = 1;
            }
        }
        else if (input_trig & PAD_SELECT) {
            if (gameState == 99){ gameState = 1;}
            else if (gameState == 1){ gameState = 99;}
        }

        //printf("Game mode %i\n", gameState);

        flushDisplay(); // dump it to the screen
        mainTimer = incTimer(mainTimer);
    }
}
