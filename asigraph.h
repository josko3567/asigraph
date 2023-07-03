/** INSCRIBING: *******************************************
 * 
 *  > PROJECT: @b ASigraph
 * 
 *  > AUTHOR:
 *      > NAME: Joško K.
 *      > EMAIL: jkriza02@fesb.hr
 * 
 *  > DOC: 30.6.2023.
 * 
 ** INTRODUCTION: *****************************************
 * 
 *  ASigraph is a cross-platform console/terminal 
 *  "graphics" library. This will be a library mostly for
 *  creating GUI's but making console games is plausible 
 *  to.
 * 
 ** COMPATIBILITY: ****************************************
 *   
 *  > C:                                                   
 *  This library is written in compliance to the limited  
 *  functionality at the disposal of the GNU C99 standard.                                                       
 *                                                       
 *  > POSIX:                                               
 *  This libraries creator has yet to inscribe the minimum
 *  POSIX compatibility version that this library requires.
 *  In its absence the user of the library is free to use 
 *  any compatibility version of the POSIX standard that is
 *  deemed to work through trial and error.               
 * 
 ** LICENSE: @c LGPLv2 ************************************
 * 
 *  This file is part of Project ASigraph, a library for 
 *  cross-platform console/terminal graphics.
 *  Copyright (C) 2023 Joško Križanović
 *   
 *  This library is free software; you can redistribute 
 *  it and/or modify it under the terms of the GNU Lesser 
 *  General Public License as published by the Free 
 *  Software Foundation; either version 2.1 of the 
 *  License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will 
 *  be useful, but WITHOUT ANY WARRANTY; without even the 
 *  implied warranty of MERCHANTABILITY or FITNESS FOR A 
 *  PARTICULAR PURPOSE. See the GNU Lesser General Public 
 *  License for more details.
 *
 *  You should have received a copy of the GNU Lesser 
 *  General Public License along with this library; if 
 *  not, write to the Free Software Foundation, Inc., 
 *  51 Franklin Street, Fifth Floor,  Boston, 
 *  MA  02110-1301  USA
 * 
 **********************************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <wchar.h>

#ifdef AG_DEV
#include "ext/viwerr/viwerr.h"
#endif

#ifndef ASIGRAPH_LIBRARY_INCLUDED
#define ASIGRAPH_LIBRARY_INCLUDED

#ifndef AG_TAB_SIZE
/**
 * @brief 
 * Tab size defines how many spaces a '\\t' takes up when
 * written to a container.
 */
#define AG_TAB_SIZE 4
#endif

/**
 * @brief 
 * Minimum amount of columns a console/terminal is allowed
 * to have.
 */
#define AG_CONSOLE_MIN_X 1

/**
 * @brief 
 * Minimum amount of rows a console/terminal is allowed
 * to have.
 */
#define AG_CONSOLE_MIN_Y 1

#define AG_CONTAINER_NAME_SIZE 256

/**
 * @brief 
 * GNU C lambdas using the anonymous function __fn__ 
 * and ({ })
 */
#define aglambda(returns, fn) \
({                            \
    returns __fn__ fn         \
    __fn__;                   \
})

typedef struct agside_st {

    uint32_t (*position)(
        uint32_t analogside, 
        uint32_t _max
    );

    uint16_t    update;
    uint16_t  priority;
    
    uint32_t  __prepos;
    uint32_t  __curpos;
    uint32_t __padding;

} agside_t;

typedef struct agcoord_st {

    uint16_t x;
    uint16_t y;

} agcoord_t;

typedef struct agcont_st {

    agside_t   left;
    agside_t  right;
    agside_t    top;
    agside_t bottom;

    struct {

        bool created;
        bool render;
        bool edges;

    } flag;

    wchar_t    *name;
    wchar_t *display;

} agcont_t;

typedef struct agterm_st {

	int a;
	int b;
	int c;

} agterm_t;


// agterm_t
uint64_t * __agframe_location(void);
#define agframe (*__agframe_location())

/**
 * @fn @c agcurmove(1)
 * 
 * @brief 
 * Move console cursor to @c coord's position.
 * 
 * @param coord
 *        (x,y) position to move the cursor to.
 * 
 * @throw Possible error return for Windows SetConsoleCursorPosition.
 *        Which is 99.9999999999999% never going to throw anything
 *        unless your install of Windows is fucked.
 * 
 * @returns Nothing.
 */
void agcurmove(
    agcoord_t coord
);

/**
 * @fn @c agcurhome(0)
 * 
 * @brief 
 * Move console cursor to (0, 0) position.
 * 
 * @throw Windows - Read agcurmove(1)
 * @throw Linux - None.
 * 
 * @returns Nothing.
 */
void agcurhome(
    void
);

/**
 * @fn @c agcurhidden(1)
 * 
 * @brief 
 * Toggle cursor visibility.
 * 
 * @throw Windows - Read agcurmove(1) but replace
 *                  SetConsoleCursorPosition with GetCursorInfo
 *                  & SetCursorInfo.
 * @throw Linux - None.
 * 
 * @returns None.
 */
void agcurhidden(
    bool hidden
);

/**
 * @fn @c agtimer(3)
 * 
 * @brief
 * agtimer() is used for maintaining a constant framerate.
 * The first call to the function only sets the last time
 * it was called. After that it gets the last time it was
 * called and the current time and checks how much it has
 * to sleep to achieve the target framerate.
 * 
 * Put it in a while loop and it will be work.
 * 
 * @param target_framerate
 *        Target framerate we are trying to achieve, the frame
 *        needs to last 1/target_framerate seconds so we sleep
 *        for the remainder of the unused processing time unless
 *        the time to process the frame was longer than
 *        1/target_framerate.
 * 
 * @param framerate
 *        Address of a double where the actual framerate
 *        is written. For no return pass NULL.
 * 
 * 
 * @param deltatime
 *        Address of a double where deltatime is written.
 *        For no return pass NULL.
 * 
 * @throw Windows - Possible QueryPerformance errors, see
 *                  agcurmove(1) for what i think on that.
 * @throw Linux - nanosleep could possibly return EINTR for
 *                interrupts but errno is reset as EINTR is
 *                handled, other than that nothing special.
 * @throw EINVAL - On both systems EINVAL is thrown if 
 *                 target_framerate is less than 0.00.
 * 
 * @return If target_framerate < 0.00 returns -1, otherwise 0.
 * 
 */
int agtimer( 
	const double target_framerate, 
	double *framerate, 
	double *deltatime 
);

/**
 * @fn @c agsleep(1)
 * 
 * @brief 
 * Sleep for a precise amount of time.
 * 
 * @param sleeptime
 *        The amount of time to sleep written in a double.
 * 
 * @throw Windows - see agtimer(1)
 * @throw Linux - see agtimer(1)
 * @throw EINVAL - If sleeptime is less than 0.00, EINVAL
 *                 is thrown
 * 
 * @return Returns -1 if sleeptime is less than 0.00, otherwise 0.
 */
int agsleep( 
	const double sleeptime 
);

// agterm_t __aginit() {

// #if defined(_WIN32) || defined(_WIN64)
// 	if(!GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &mode)){
// 		DWORD err = GetLastError();
// 		printf("Last error = %d", err);
// 		return 0;
// 	}
// 	if(!SetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), mode | ENABLE_VIRTUAL_TERMINAL_INPUT)){
// 		DWORD err = GetLastError();
// 		printf("Last error = %d", err);
// 		return 0;
// 	}
// #endif

// }

#endif /** @c ASIGRAPH_LIBRARY_INCLUDED */