
#include "sintimer.h"

////////////////////////////////////////////////////////////////////////////////
//

void sinTimer::init(int in) {
        //convert interval from event/sec to millisec/event
        d_interval = (1.0/(float)in);
        d_currentTime = glfwGetTime();
        d_previousTime = d_currentTime; }

void sinTimer::initMS(int in) {
        //input is in milliseconds
        d_interval = (double)in/1000.0;
        d_currentTime = glfwGetTime();
        d_previousTime = d_currentTime; }

int  sinTimer::needUpdate() {
        if ( glfwGetTime() - d_previousTime >= d_interval ) {
            d_previousTime += d_interval;
            return 1; }
        return 0; }

int  sinTimer::needUpdateNoCarry() {
        if ( glfwGetTime() - d_previousTime >= d_interval ) {
            d_previousTime = glfwGetTime();
            return 1; }
        return 0; }

void sinTimer::catchup() { d_previousTime = glfwGetTime(); }
void sinTimer::reset()   { d_previousTime = glfwGetTime(); }

sinTimer::sinTimer() {}
sinTimer::sinTimer(int in) { init(in); }


////////////////////////////////////////////////////////////////////////////////
//

double sinTimerCounter::timeSinceLastCall() {
        d_elapsedTime = glfwGetTime() - d_currentTime;
        d_currentTime = glfwGetTime();
        return d_elapsedTime; }

sinTimerCounter::sinTimerCounter() { d_currentTime = glfwGetTime(); }

