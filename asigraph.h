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

uint64_t * __agframe_location(void);
#define agframe (*__agframe_location())

bool agcurmove(agcoord_t coord);

bool agtimer( 
	const double target_framerate, 
	double *framerate, 
	double *deltatime 
);

bool agsleep( 
	const double sleeptime 
);

#endif /** @c ASIGRAPH_LIBRARY_INCLUDED */