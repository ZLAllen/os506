#include <sys/ktime.h>
#include <sys/kprintf.h>
#include <sys/system.h>

#define CH0 0x40
#define CMD 0x43
#define MOD3 0x36 //Load Low then high, use mode 3 square wave


int ticks;
int s_cnt; // every 5 seconds, we wait for 38 ticks
uint64_t second;
uint64_t day;

extern uint64_t ms; // total elapsed ms, used for sleep

void init_ktime(){
    outb(CMD, MOD3);
    outb(CH0, 0x00);
    outb(CH0, 0x00);
}

void addTick(){
   //kputs("ticks\n");
    if(s_cnt < 4) {
        if(ticks < 18) {
            ++ticks;
            ms += 55; // 1000 / 18
        } else {
            ms += 10; // 55 * 18 = 990
            ++second;
            ticks = 0;
            ++s_cnt;
            update_time(day, second);
        }
    }else{
        if(ticks < 19) {
            ++ticks;
            ms += 52; // 1000 / 19
        } else {
            ms += 12; // 52 * 19 = 988
            ++second;
            ticks = 0;
            s_cnt = 0;
            update_time(day, second);
        }
    }

    if(second >= 86400){
        day++;
        second = 0;
        update_time(day, second);
    }
}


