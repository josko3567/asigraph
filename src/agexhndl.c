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

    // struct sigaction act = {

    //     .sa_handler  = sigfunc,
    //     .sa_flags    = 0,
    //     .sa_mask     = {0},
    //     .sa_restorer = NULL

    // };

    // Add default action to exit's.
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
    // signal
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
//     agtermecho(true); 
//     agtermcurhidden(false);
//     endwin();
//     // agcont_t * cont = __cont;
//     // __builtin_dump_struct(cont, &printf);
//     if(__cont != NULL){
//         free(__cont->display._1D);
//         free(__cont->display._2D);
//         free(__cont->name);
//         free(__cont);
//     }
// #ifdef AG_DEV
//     switch(agexhndlsig) {
//         case SIGINT:
//         printf("Ctrl-C exit.\n");
//         break;

//         case SIGSEGV:
//         printf("Segfault occurred!\n");
//         break;

//         default:
//         if(agexhndlsig != 0) printf("%d\n", agexhndlsig);
//         break;
//     }
// #endif
//     while(viwerr(VIWERR_OCCURED, NULL)){
//     	viwerr(VIWERR_PRINT, NULL);
//     }

}
