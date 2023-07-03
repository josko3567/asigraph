#include <stdio.h>
#include <errno.h>
#include "../asigraph.h"
#include "../ext/viwerr/viwerr.h"

int main(void) {
	
	atexit(aglambda(void, (void){
		agcurhidden(false);
		fprintf(stdout, "\x1b[2J");
	}));

        double framerate, deltatime, target = 60.0;
        fprintf(stdout, "\x1b[2J");
        agcurhidden(true);
        while(agtimer(target, &framerate, &deltatime)) {
                agcurmove((agcoord_t){.x = 0, .y = 0});
                fprintf(stdout, "Target = %f, Current = %f, Delta = %f\n",
                        target,
                        framerate,
                        deltatime
                );
                viwerr(VIWERR_PRINT, NULL);
        }
        agcurhidden(false);
        return 0;

}