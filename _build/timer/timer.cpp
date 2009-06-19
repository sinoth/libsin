
#include "timer.h"

////////////////////////////////////////////////////////////////////////////////
//

void timer::init(int in) {
        //convert interval from event/sec to millisec/event
        d_interval = (1.0/(float)in);
        d_currentTime = glfwGetTime();
        d_previousTime = d_currentTime; }

int  timer::needUpdate() {
        if ( glfwGetTime() - d_previousTime >= d_interval ) {
            d_previousTime += d_interval;
            return 1; }
        return 0; }

int  timer::needUpdateNoCarry() {
        if ( glfwGetTime() - d_previousTime >= d_interval ) {
            d_previousTime += d_interval;
            return 1; }
        return 0; }

void timer::catchup() { d_previousTime = glfwGetTime(); }

timer::timer() {}
timer::timer(int in) { init(in); }


////////////////////////////////////////////////////////////////////////////////
//

double timerCounter::timeSinceLastCall() {
        d_elapsedTime = glfwGetTime() - d_currentTime;
        d_currentTime = glfwGetTime();
        return d_elapsedTime; }

timerCounter::timerCounter() { d_currentTime = glfwGetTime(); }

