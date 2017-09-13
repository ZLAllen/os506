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

void init_ktime(){
    outb(CMD, MOD3);
    outb(CH0, 0x00);
    outb(CH0, 0x00);
}

void addTick(){
   //kputs("ticks\n");
    if(s_cnt < 4){
        if(ticks < 18)
            ++ticks;
        else{
            ++second;
            ticks = 0;
            ++s_cnt;
            kputs("18 ticks\n");
            update_time(day, second);
        }
    }else{
        if(ticks < 19)
            ++ticks;
        else{
            ++second;
            ticks = 0;
            s_cnt = 0;
            kputs("19 ticks\n");
            update_time(day, second);
        }
    }

    if(second >= 86400){
        day++;
        second = 0;
        update_time(day, second);
    }
}




