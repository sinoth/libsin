#ifndef TIMER_H
#define TIMER_H

#include <GL/glfw.h>

////////////////////////////////////////////////////////////////////////////////
// timer declaration
//

class timer {
public:

    void init(int in);
    int  needUpdate();
    int  needUpdateNoCarry();
    void catchup();

    timer();
    timer(int in);

private:

    double d_interval;
    double d_currentTime;
    double d_previousTime;

};



////////////////////////////////////////////////////////////////////////////////
// counter declaration
//

class timerCounter {
public:

    double timeSinceLastCall();
    timerCounter();

private:

    double d_elapsedTime;
    double d_currentTime;

};


#endif //\/ TIMER_H
