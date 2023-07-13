#include <stdio.h>
#include <stdbool.h>
#include "../asigraph.h"

/**
 * @fn @c agcurmove(1)
 * 
 * @brief 
 * Move console cursor to @c coord's position.
 */
void agcurmove(agcoord_t coord) {
        
        fprintf(stdout, "\x1b[%d;%dH", coord.y, coord.x);

}



/**
 * @fn @c agcurhome(0)
 * 
 * @brief 
 * Move console cursor to (0, 0) position.
 */
void agcurhome(void) {

        fprintf(stdout, "\x1b[H");

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
