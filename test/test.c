#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <wchar.h>
#include <string.h>
#include "../agansicode.h"
#include "../asigraph.h"
#include "../ext/viwerr/viwerr.h"
#include <locale.h>

int main(void) {

    aginit((aginit_arg){0});
    int br = baudrate();

    __cont = agcontinit((agcont_t){
        .left = {
            .position = ^uint32_t(uint32_t right, uint32_t xmax) {
                return agtermlimits().x.min;
            },
            .padding = 0
        },
        .right = {
            .position = ^uint32_t(uint32_t left, uint32_t xmax) {
                return agtermlimits().x.max-1;
            },
            .padding = 0
        },
        .top = {
            .position = ^uint32_t(uint32_t bottom, uint32_t ymax) {
                return agtermlimits().y.min;
            },
            .padding = 0
        },
        .bottom = {
            .position = ^uint32_t(uint32_t top, uint32_t ymax) {
                return agtermlimits().y.max-1;
            },
            .padding = 0
        },
        .name = L"Temporary Container.",
        .border = AG_CONTAINER_BORDER_DEFAULT,
        .flag.render = true
    });

    wchar_t buffer[128] = {0};
    double framerate, deltatime, target = 30.0;

    while(agtimer(target, &framerate, &deltatime) == 0) {

        if(agframe >= 12000) break;
		// resize_term(0,0);
        if(agtermsizechanged == true){
			printw("Hello");
            agtermclear();
            __cont->update.all(__cont);
        }

        swprintf(buffer, 127, L"Framerate: %lf, %d, %d", 
            framerate, br, agframe);
        wcsncpy((wchar_t*)(__cont->display._2D[1]+1), 
            buffer,
            wcsnlen(buffer, 127) > AG_CONTAINER_X_LENGTH(__cont)-2 ? 
                AG_CONTAINER_X_LENGTH(__cont)-2:
                wcsnlen(buffer, 127)
        );
  
        agcontdraw(__cont, (agcoord_t){
            .x = 0,
            .y = 0,
            // .x = (COLS / 2) - (__cont->length.x / 2),
            // .y = (LINES / 2) - (__cont->length.y / 2),
        });

		// if( __cont->length.x >= 4
		// &&  __cont->length.y >= 4) {

		// 	printw("%d\n", __cont->display._2D[1][1]);

		// }
        
        agtermrefresh();
        agframe += 1;
        
    }



    return 0;

}
