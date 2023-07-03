#include <stdio.h>
#include <errno.h>
#include <Windows.h>
#include "../asigraph.h"
#include "../ext/viwerr/viwerr.h"

int main(void) {
	
	// HANDLE hndl = GetStdHandle(STD_OUTPUT_HANDLE);
	DWORD mode;
	if(!GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &mode)){
		DWORD err = GetLastError();
		printf("Last error = %d", err);
		return 0;
	}
	if(!SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), mode | ENABLE_VIRTUAL_TERMINAL_INPUT)){
		DWORD err = GetLastError();
		printf("Last error = %d", err);
		return 0;
	}

	mode = 0;
	GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &mode);
	SetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);

	atexit(aglambda(void, (void){
		agcurhidden(false);
		// fprintf(stdout, "\x1b[2J");
		puts("\e[?1049l");
	}));

        double framerate, deltatime, target = 60.0;
	puts("\x1b[?1049h");
        agcurhidden(true);
        puts("\x1b[2J");
        while(agtimer(target, &framerate, &deltatime) == 0) {
                agcurmove((agcoord_t){.x = 0, .y = 0});
                fprintf(stdout, "Target = %f, Current = %f, Delta = %f\n",
                        target,
                        framerate,
                        deltatime
                );
                viwerr(VIWERR_PRINT, NULL);
        }
        // agcurhidden(false);
        return 0;

}