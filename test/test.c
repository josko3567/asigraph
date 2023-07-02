#include <stdio.h>
#include <errno.h>
#include "../asigraph.h"
#include "../ext/viwerr/viwerr.h"

int main(void) {
	
	// errno = 100;
        // agframe += 100;
        // fprintf(stdout, "\x1b[2J");
        double framerate, deltatime, target = 60.0;
        // errno = 1;
        // errno = 1;
        // viwerr(VIWERR_PUSH, &(viwerr_package){
        //         .group = (char*)"errno",
        //         .code = EINVAL
        // });

        // printf("%d\n", errno);
        agsleep(3.0000);
        // while(agtimer(target, &framerate, &deltatime)) {
                fprintf(stdout, "\x1b[2J");
                // agcurmove((agcoord_t){.x = 0, .y=0});
                // fprintf(stdout, "Target = %f, Current = %f, Delta = %f\n",
                        // target,
                        // framerate,
                        // deltatime
                // );
                // viwerr(VIWERR_PRINT, NULL);
        // }
	// while(viwerr(VIWERR_OCCURED, NULL) ){
        //         viwerr(VIWERR_PRINT, NULL);
        // }
        // printf("%d\n", agframe);
        return 0;

}