/*
 * timerz.h
 *  Re-Created on: Dec 19, 2020
 *      Author: NDR008
 */

#pragma once

typedef struct PSXTimer {
	int min;
	int sec;
	int vsync;
	int factor;
    int totalsec;
} PSXTimer;

PSXTimer createTimer();
PSXTimer incTimer(PSXTimer timer);