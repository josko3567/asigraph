#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <stdbool.h>
#include "../agansicode.h"
#include "../asigraph.h"
#include "../ext/viwerr/viwerr.h"

int * __agexhndl_signal() {

    static int sig = 0;
    return &sig;

}

// Add new handler to all exit handlers.
void agexhndladd(
    void (*sigfunc)(int),
    void (*atexitfunc)(void))
{

    struct sigaction act = {

        .sa_handler  = sigfunc,
        .sa_flags    = 0,
        .sa_mask     = {0},
        .sa_restorer = NULL

    };

    // Add default action to exit's.
    sigaction(SIGABRT, &act, NULL);
    sigaction(SIGINT , &act, NULL);
    sigaction(SIGQUIT, &act, NULL);
    sigaction(SIGTERM, &act, NULL);
    sigaction(SIGHUP , &act, NULL);
    sigaction(SIGSEGV, &act, NULL);
    sigaction(SIGILL , &act, NULL);
    atexit(atexitfunc);

    return;       

}

// Default exit handler.
void __agexhndl_abnormal(int sig) {

    agexhndlsig = sig;
    exit(EXIT_SUCCESS);

}

void __agexhndl_normal(void) {

    agtermecho(true); 
    agtermcurhidden(false);
    endwin();
    // agcont_t * cont = __cont;
    // __builtin_dump_struct(cont, &printf);
    if(__cont != NULL){
        free(__cont->display._1D);
        free(__cont->display._2D);
        free(__cont->name);
        free(__cont);
    }
#ifdef AG_DEV
    switch(agexhndlsig) {
        case SIGINT:
        printf("Ctrl-C exit.\n");
        break;

        case SIGSEGV:
        printf("Segfault occurred!\n");
        break;

        default:
        if(agexhndlsig != 0) printf("%d\n", agexhndlsig);
        break;
    }
#endif
    while(viwerr(VIWERR_OCCURED, NULL)){
    	viwerr(VIWERR_PRINT, NULL);
    }

}
