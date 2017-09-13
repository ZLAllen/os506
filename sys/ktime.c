#include <sys/ktime.h>

int ticks;
int s_cnt; // every 5 seconds, we wait for 19 ticks
uint64_t second;
uint64_t day;

void addTick(){
    if(s_cnt < 5){
        if(ticks < 18)
            ++ticks;
        else{
            ++second;
            ticks = 0;
            ++s_cnt;
        }
    }else{
        if(ticks < 19)
            ++ticks;
        else{
            ++second;
            ticks = 0;
            s_cnt = 0;
        }
    }

    if(second >= 86400){
        day++;
        second = 0;
    }
}




