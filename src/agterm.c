#include <stdio.h>
#include <stdbool.h>
#include <errno.h>
#include "../ext/viwerr/viwerr.h"
#include "../asigraph.h"

#include <ncursesw/form.h>

int agtermrefresh(void) {
	
    return refresh();

}

int agtermclear(void){

    return clear();

}

/**
 * @brief 
 * Enable echo in the terminal.
 */
int agtermecho(
    bool echos)
{

    return echos ? echo() : noecho();    

}

__always_inline
agtermlimit_t agtermlimits(void) {

    return (agtermlimit_t){
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
