#define _POSIX
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

void agexhndl( 
    enum agexhndl_args_en arg,
    int signal,
    void (^handler)(int))
{

    static void (^current)(int) = NULL; 

    switch(arg) {

        case AGEXHNDL_ADD:
        current = handler;
        break;

        case AGEXHNDL_RUN:
        if(current == NULL)
            current = AG_DESTRUCTOR_DEFAULT;
        current(signal);
        break;

        default:
        break;
    
    }

}

// Add new handler to all exit handlers.
void agexhndladd(
    void (*sigfunc)(int),
    void (*atexitfunc)(void))
{

    signal(SIGABRT, sigfunc);
    signal(SIGINT , sigfunc);
    signal(SIGTERM, sigfunc);
    signal(SIGSEGV, sigfunc);
    signal(SIGILL , sigfunc); 
#ifdef SIGQUIT
    signal(SIGQUIT, sigfunc);
#endif
#ifdef SIGHUP
    signal(SIGHUP , sigfunc);
#endif
    atexit(atexitfunc);

    return;       

}

// Default exit handler.
// Totaly get it.
void __agexhndl_abnormal(int sig) {

    agexhndlsig = sig;
    exit(EXIT_SUCCESS);

}

// These are phony exit handlers that push all the exit handling to agexhndl()
void __agexhndl_normal(void) {


    agexhndl(
        AGEXHNDL_RUN,
        agexhndlsig,
        NULL
    );

}
