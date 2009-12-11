#ifndef TIMER_H
#define TIMER_H

#include <GL/glfw.h>

////////////////////////////////////////////////////////////////////////////////
// timer declaration
//

class sinTimer {
public:

    void init(int in) {
            //convert interval from event/sec to millisec/event
            d_interval = (1.0/(float)in);
            d_currentTime = glfwGetTime();
            d_previousTime = d_currentTime; }

    void initMS(int in) {
            //input is in milliseconds
            d_interval = (double)in/1000.0;
            d_currentTime = glfwGetTime();
            d_previousTime = d_currentTime; }

    int  needUpdate() {
            if ( glfwGetTime() - d_previousTime >= d_interval ) {
                d_previousTime += d_interval;
                return 1; }
            return 0; }

    int  needUpdateNoCarry() {
            if ( glfwGetTime() - d_previousTime >= d_interval ) {
                d_previousTime = glfwGetTime();
                return 1; }
            return 0; }

    void catchup() { d_previousTime = glfwGetTime(); }
    void reset() { d_previousTime = glfwGetTime(); }

    sinTimer() {}
    sinTimer(int in) { init(in); }

private:

    double d_interval;
    double d_currentTime;
    double d_previousTime;

};



////////////////////////////////////////////////////////////////////////////////
// counter declaration
//

class sinTimerCounter {
public:

    double timeSinceLastCall() {
            d_elapsedTime = glfwGetTime() - d_currentTime;
            d_currentTime = glfwGetTime();
            return d_elapsedTime; }

    sinTimerCounter() { d_currentTime = glfwGetTime(); }

private:

    double d_elapsedTime;
    double d_currentTime;

};


#endif //\/ TIMER_H
