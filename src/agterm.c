#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include "../ext/viwerr/viwerr.h"
#include "../asigraph.h"

int agtermrefresh(void) {
	
    return refresh();

}

int agtermclear(void){

    return clear();

}

int agtermecho(
    bool echos)
{

    return echos ? echo() : noecho();    

}

inline __attribute__((always_inline))
aglimit_t agtermlimits(void) {

    return (aglimit_t){
        .x = {
            .min = 0,
            .max = COLS
        },
        .y = {
            .min = 0,
            .max = LINES
        }
    };
    
}

bool __agtermsizechanged(void) {
    static aglimit_t prev = {0};
    static bool start = true;
    if(start) {
        prev = agtermlimits();
        start = false;
        return false;
    }

    if(prev.x.max != agtermlimits().x.max
    || prev.y.max != agtermlimits().y.max){
        prev = agtermlimits();
        return true;
    }

    prev = agtermlimits();
    return false;

}
