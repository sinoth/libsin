
#ifdef _WIN32_WINNT
 #include <windows.h>
#elif _LINUX
 #include <inttypes.h>
 #include <sys/time.h>
#endif

class sinTimer {
public:

    bool init(int in) {
          #ifdef _WIN32_WINNT
            __int64 freq;
            if( !QueryPerformanceFrequency( (LARGE_INTEGER *)&freq ) ) {
                //too bad, no counter for you
                return 1; }
            interval = ( 1.0 / (double)in ) * (double)freq;
            QueryPerformanceCounter( (LARGE_INTEGER *)&new_tick_count );
            old_tick_count = new_tick_count;
            return 0;
          #elif _LINUX
            gettimeofday(&current_time,NULL);
            old_tick_count = current_time.tv_sec*1000000 + current_time.tv_usec;
            interval = ( 1000000.0 / (double)in );
            return 0;
          #endif
        }


    int  needUpdate() {
          #ifdef _WIN32_WINNT
            QueryPerformanceCounter( (LARGE_INTEGER *)&new_tick_count );
            if ( new_tick_count - old_tick_count >= interval ) {
                old_tick_count += interval;
                return 1; }
            return 0;
          #elif _LINUX
            gettimeofday(&current_time, NULL);
            if ( (current_time.tv_sec*1000000 + current_time.tv_usec) - old_tick_count >= interval ) {
                old_tick_count += interval;
                return 1; }
            return 0;
          #endif
        }

    int  needUpdateNoCarry() {
          #ifdef _WIN32_WINNT
            QueryPerformanceCounter( (LARGE_INTEGER *)&new_tick_count );
            if ( new_tick_count - old_tick_count >= interval ) {
                old_tick_count = new_tick_count;
                return 1; }
            return 0;
          #elif _LINUX
            gettimeofday(&current_time, NULL);
            if ( (current_time.tv_sec*1000000 + current_time.tv_usec) - old_tick_count >= interval ) {
                old_tick_count = (current_time.tv_sec*1000000 + current_time.tv_usec);
                return 1; }
            return 0;
          #endif
        }

    void reset() {
          #ifdef _WIN32_WINNT
            QueryPerformanceCounter( (LARGE_INTEGER *)&new_tick_count );
            old_tick_count = new_tick_count;
          #elif _LINUX
            gettimeofday(&current_time, NULL);
            old_tick_count = (current_time.tv_sec*1000000 + current_time.tv_usec);
          #endif
        }

    sinTimer() {}
    sinTimer(int in) { init(in); }

private:

    #ifdef _WIN32_WINNT
      __int64 old_tick_count;
      __int64 new_tick_count;
      __int64 interval;
    #elif _LINUX
      timeval current_time;
      uint64_t old_tick_count;
      uint64_t interval;
    #endif

};

