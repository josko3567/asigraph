#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include <sys/prctl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#endif

#include "../agansicode.h"
#include "../asigraph.h"
#include "../ext/viwerr/viwerr.h"
#include "../ext/obj.h/obj.h"

// struct Block_layout {
//     void *isa;
//     int flags;
//     int reserved; 
//     void (*invoke)(void *, ...);
//     struct Block_descriptor *descriptor;
// };




int main(void) {

        aginit((aginit_arg){0});
        
#if defined(_WIN32) || defined(_WIN64)

#else
	struct winsize w;
    	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
#endif
	// void (^atexitf)(void) = ^{printf("Hello world!");};
	// atexit(((void)(*)(void))(((struct Block_layout *)atexitf`)->invoke));


        double framerate, deltatime, target = 60.0;
	// agtermbuffer(AG_ALT_BUFFER);
        // agcurhidden(true);
	// puts("\x1b[1J");
        while(agtimer(target, &framerate, &deltatime) == 0) {

                // ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
                if(agframe >= 1200000) break;
		agcurmove((agcoord_t){.x=0,.y=0});
                fprintf(stdout, 
                        "Target = %.3f, "
                        "Current = %.5f, "
                        "Delta = %.5f, "
                        "Frame = %lu\n",
                        target,
                        framerate,
                        deltatime,
                        agframe
                );
		const char * str= "Hello!\n";
                agcurmove((agcoord_t){
			.x = w.ws_col/2-(strlen(str)/2), 
			.y = w.ws_row/2
		});
		printf("%s", str);
                agframe += 1;

        }
	// puts("\x1b[1J");
	// agtermbuffer(AG_MAIN_BUFFER);
        // agcurhidden(false);

        return 0;

}
