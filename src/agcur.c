#include <stdio.h>
#include <stdbool.h>
#include "../asigraph.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <winerror.h>
#endif

/**
 * @fn @c agcurmove(1)
 * 
 * @brief 
 * Move console cursor to @c coord's position.
 */
void agcurmove(agcoord_t coord) {
        
#if defined(_WIN32) || defined(_WIN64)
        SetConsoleCursorPosition(
                GetStdHandle(STD_OUTPUT_HANDLE),
                (COORD){coord.x, coord.y}
        );
#else
        fprintf(stdout, "\x1b[%d;%dH", coord.y, coord.x);
#endif

}



/**
 * @fn @c agcurhome(0)
 * 
 * @brief 
 * Move console cursor to (0, 0) position.
 */
void agcurhome(void) {

#if defined(_WIN32) || defined(_WIN64)
        SetConsoleCursorPosition(
                GetStdHandle(STD_OUTPUT_HANDLE),
                (COORD){0, 0}
        );
#else
        fprintf(stdout, "\x1b[H");
#endif

}

/**
 * @fn @c agcurhidden(1)
 * 
 * @brief 
 * Toggle cursor visibility.
 */
void agcurhidden(bool hidden) {

        if(hidden) fprintf(stdout,"\x1b[?25l");
        else fprintf(stdout,"\x1b[?25h");

}
