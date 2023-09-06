#include <stdio.h>
#include <stdbool.h>
#include "../asigraph.h"

/**
 * @fn @c agtermcurmove(1)
 * 
 * @brief 
 * Move console cursor to @c coord's position.
 */
int agtermcurmove(agcoord_t coord) {
        
	return move(coord.y, coord.x);
        
}

/**
 * @fn @c agtermcurpos(0)
 * 
 * @brief 
 * Get console cursor position.
 */
agcoord_t agtermcurpos(void) {

    return (agcoord_t){.x = getcurx(stdscr), .y = getcury(stdscr)};

}

/**
 * @fn @c agtermcurhome(0)
 * 
 * @brief 
 * Move console cursor to (0, 0) position.
 */
int agtermcurhome(void) {

    return move(1,1);

}

/**
 * @fn @c agtermcurhidden(1)
 * 
 * @brief 
 * Toggle cursor visibility.
 */
void agtermcurhidden(bool hidden) {

    if(hidden) curs_set(0);
    else       curs_set(1);

}
