#include <stdio.h>
#include <errno.h>
#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include <sys/prctl.h>
#endif

#include "../asigraph.h"
#include "../ext/viwerr/viwerr.h"

void foo(void (*bar)(void)) {
        bar();
}

int main(void) {
	
#if defined(_WIN32) || defined(_WIN64)
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
#endif

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
                agframe += 1;
                if(agframe >= 120) break;
        }
	puts("\e[?1049l");
        agcurhidden(false);

        // printf("6+6=%d\n",add(6,6));

        return 0;

}