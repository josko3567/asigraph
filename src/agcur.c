#include <stdio.h>
#include <stdbool.h>
#include "../asigraph.h"

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <winerror.h>
#else

#endif

/**
 * @fn @c agcurmove(1)
 * 
 * @brief 
 * Move console cursor to @c coord's position.
 */
bool agcurmove(agcoord_t coord) {
        
#if defined(_WIN32) || defined(_WIN64)
        if( !SetConsoleCursorPosition(
                GetStdHandle(STD_OUTPUT_HANDLE),
                (COORD){coord.x, coord.y}
        )) {
                return false;
        }
        return true;
#else
        fprintf(stdout, "\x1b[%d;%dH", coord.y, coord.x);
        return true;
#endif


}