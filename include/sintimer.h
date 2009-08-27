#ifndef TIMER_H
#define TIMER_H

#include <GL/glfw.h>

////////////////////////////////////////////////////////////////////////////////
// timer declaration
//

class sinTimer {
public:

    void init(int in);
    int  needUpdate();
    int  needUpdateNoCarry();
    void catchup();

    sinTimer();
    sinTimer(int in);

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

    double timeSinceLastCall();
    sinTimerCounter();

private:

    double d_elapsedTime;
    double d_currentTime;

};


#endif //\/ TIMER_H
