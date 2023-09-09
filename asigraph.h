/**
 >> INSCRIBING: >-----------------------------------------<
 * 
 *  > PROJECT: @b ASigraph
 * 
 *  > AUTHOR:
 *      > NAME: Joško K.
 *      > EMAIL: jkriza02@fesb.hr
 * 
 *  > DOC: 30.6.2023.
 * 
 >> INTRODUCTION: >---------------------------------------<
 * 
 *  ASigraph is a cross-platform console/terminal 
 *  "graphics" library. This will be a library mostly for
 *  creating GUI's but making console games is plausible 
 *  to.
 * 
 >> COMPATIBILITY: >--------------------------------------<
 *   
 *  > C:                                                   
 *  This library is written in compliance to the available  
 *  functionality at the disposal of the GNU C11 standard.
 *  Additional features only available with the clang
 *  compiler are present in the code (clang blocks).
 *  It is an apsolute requirement that you compile the
 *  project with clang and with the arguments "-fblocks 
 *  -lBlocksRuntime"  
 *                                                       
 *  > POSIX:                                               
 *  This libraries creator has yet to inscribe the minimum
 *  POSIX compatibility version that this library requires.
 *  In its absence the user of the library is free to use 
 *  any compatibility version of the POSIX standard that is
 *  deemed to work through trial and error.               
 * 
 >> LICENSE: @c LGPLv2 >----------------------------------<
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
 << >--------------------------------------------------< */

#include <stdio.h>
#include <stdint.h>
#include <wchar.h>
#include <stdbool.h>
#include <signal.h>

#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#	define AG_PLATFORM_WINDOWS
#	define AG_PLATFORM "Windows"
#elif defined(__linux__)
#	define AG_PLATFORM_LINUX
#	define AG_PLATFORM "Linux"
#else
#	define AG_PLATFOR_UNKNOWN
#	define AG_PLATFORM "Unknown"
#endif

#if defined(AG_PLATFORM_WINDOWS)
#	ifndef PDC_WIDE
#		define PDC_WIDE
#	endif
#	include "./ext/PDCurses/curses.h"
#elif defined(AG_PLATFORM_LINUX)
#	ifndef NCURSES_WIDECHAR
#		define NCURSES_WIDECHAR 1
#	endif
#	include <ncursesw/ncurses.h>
#elif AG_PLATFORM_UNKNOWN
#	ifndef NCURSES_WIDECHAR
#		define NCURSES_WIDECHAR 1
#	endif
#	include <ncursesw/ncurses.h>
#	pragma message ("Warning from asigraph:" \
	" Currently compiling for a unknown platform." \
	" Supported platforms are Windows and Linux.")
#endif

#include "agansicode.h"
#include "ext/viwerr/viwerr.h"

#ifndef __BLOCKS__
#	error must be compiled with -fblocks option enabled.
#endif

#ifndef ASIGRAPH_LIBRARY_INCLUDED
#define ASIGRAPH_LIBRARY_INCLUDED

#define AG_ERROR_GROUP (char*)"asigraph"

/**
 * @brief 
 * Exit code for aginit()
 */
#define AG_EXIT_CODE 79

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

/**
 * @brief 
 * Max name size.
 */
#define AG_CONTAINER_NAME_SIZE 256

#define AG_CONTAINER_X_LENGTH(cont) \
    ((cont->right.__curpos - cont->left.__curpos)+1)

#define AG_CONTAINER_Y_LENGTH(cont) \
    ((cont->bottom.__curpos - cont->top.__curpos)+1)

/**
 * @brief 
 * Calculate how much to allocate for the display.
 */
#define AG_CONTAINER_DISPLAY_SIZE(cont) \
    (sizeof(wchar_t)                    \
    * AG_CONTAINER_X_LENGTH(cont)       \
    * AG_CONTAINER_Y_LENGTH(cont)) 

/**
 * @brief 
 * Identifier for object types.
 * Written into <object>->object.id.type;
 */
typedef enum agobjtype_en {

    AG_OBJECT_TYPE_UNKNOWN,
    AG_OBJECT_TYPE_STDSCR,
    AG_OBJECT_TYPE_CONTAINER

} agobjtype_t;

/**
 * @brief 
 * Coordinate point structure.
 * Any function that needs a x,y position will be given it in
 * agcoord_t structure.
 */
typedef struct agcoord_st {

    int32_t x;
    int32_t y;

} agcoord_t;

/**
 * @typedef @c agobj_t
 * 
 * @brief 
 * Everything that can be considered a object contains a agobj_t identifier
 * at the beginning of the object structure. This allows asigraph to identify
 * the object without knowing what type of object it is (container, menu,
 * etc...).
 * To be more precise...
 * anything that can be displayed and/or can be a child of a another object
 * has a agobj_t at the beginning of its structure.
 * 
 * @param id.type - identifier for the object type.
 * @param id.unique - unique identifier for the created object, usually
 *                    it's the number of created objects of the same type
 *                    when the object was being initalized aka. created.  
 * 
 * @param name - name of the object type.
 * @param description - further description of the object type.
 * 
 * @param is.drawable - Specifies wether or not a object is meant to be drawn
 *                      onto the screen. If true object.draw() is not a NULL
 *                      pointer.
 * @param is.attribute - Specifies if the object is a attribute for it's
 *                       parent object.
 * 
 * @param draw() - 
 * @param inject() - 
 * 
 * @param event.clicked() - 
 * 
 */
typedef struct agobj_st {

    struct {
        agobjtype_t type;
        uint64_t  unique;
    } id;

    struct {

        bool drawable;
        bool attribute;

    } is;

    int (*draw)(
        struct agobj_st *object
    );    

    int (*inject)(
        struct agobj_st *parent
    );

    struct {

        int (*clicked)(
            agcoord_t coord
        );

    } event;

    char *name;
    char *description;    

} agobj_t;

/**
 * @brief 
 * Predefined values for agobj_t object structure inside of a agcont_t
 * structure. Do not use .id.unique for setup, agcontinit gives the 
 */
#define AG_OBJECT_CONTAINER (agobj_t){                                         \
    .id = {                                                                    \
        .type = AG_OBJECT_TYPE_CONTAINER,                                      \
        .unique = UINT64_MAX                                                   \
    },                                                                         \
    .is = {                                                                    \
        .drawable = true,                                                      \
        .attribute = false                                                     \
    },                                                                         \
    .name = (char*)"agcont_t: container",                                      \
    .description = (char*)"Default container type for displaying text "        \
    "to the screen.",                                                          \
    .draw   = NULL,                                                            \
    .inject = NULL,                                                            \
    .event  = {                                                                \
        .clicked = NULL                                                        \
    }                                                                          \
}

#define AG_OBJECT_STDSCR (agobj_t){                                            \
    .id = {                                                                    \
        .type = AG_OBJECT_TYPE_STDSCR,                                         \
        .unique = UINT64_MAX                                                   \
    },                                                                         \
    .is = {                                                                    \
        .drawable = false,                                                     \
        .attribute = false                                                     \
    },                                                                         \
    .name = (char*)"ncurses stdscr.",                                          \
    .description = (char*)"For all objects that have \"no parent object\" have"\
            " stdscr as their parent. Not using NULL because uninitialized"    \
            " or freed objects have parents set as NULL."                      \
    .draw = NULL,                                                              \
    .inject = NULL,                                                            \
    .event = {                                                                 \
        .clicked = NULL                                                        \
    }                                                                          \
}

typedef struct agtermlimit_st {

    struct {
        uint32_t min;
        uint32_t max;
    } x;

    struct {
        uint32_t min;
        uint32_t max;
    } y;

} agtermlimit_t;

/**
 * @typedef @c agside_t
 * 
 * @brief 
 * Defines where one of the sides of a agcont_t will be positioned.
 * 
 * @param position clang block value type, blocks are basically 
 *                 lambdas so their functionality is very useful.
 * 
 * @tparam position(analogside) analogside is the current position
 *                              of the analog side, duh.
 *                              For top thats bottom and vice versa.
 *                              For left thats right and vice versa.
 * 
 * @tparam position(_max) the _ in _max can mean either xmax or ymax.
 *                        xmax for left and right sides,
 *                        ymax for top and bottom sides.
 *                        
 * @param priority because @a position() accepts the analogside, there
 *                 are 2 things to keep in mind.
 *                 1. You cannot use the analog side position in
 *                    two sides that are analog to each other at the same
 *                    time.
 *                    Meaning if you have top and bottom side, either
 *                    top and bottom dont use one another in their position
 *                    formula, top uses bottom's position or bottom uses 
 *                    top's position. You cannot have a instance where
 *                    top uses bottom's position and bottom uses top's
 *                    position at the same time. This will cause undefined
 *                    behavior because the asigraph doesn't know how to
 *                    order them by priority.
 *                 2. If one side uses the analog sides position and the other
 *                    one doesn't, priority will be set up to update first
 *                    the independent side and then the dependant side.
 *                 This is what priority is used for.
 * 
 * @param padding additional padding aka. space between the writable space and
 *                the border.
 *                    
 * 
 */
typedef struct agside_st {

    uint32_t (^position)(
        uint32_t analogside, 
        uint32_t _max
    );

    uint16_t  priority;
    uint32_t   padding;
    
    uint32_t  __prepos;
    uint32_t  __curpos;

} agside_t;

typedef struct agcontborder_st {

    bool draw;
    struct {

        wchar_t    top;
        wchar_t bottom;
        wchar_t   left;
        wchar_t  right;
        wchar_t     tr; 
        wchar_t     tl; 
        wchar_t     br; 
        wchar_t     bl;

    } icon;

} agcontborder_t;

#define AG_CONTAINER_BORDER_DEFAULT (agcontborder_t){ \
    .draw = true,                                     \
    .icon = {                                         \
        .bottom   = L'\u2500',                        \
        .top      = L'\u2500',                        \
        .left     = L'\u2502',                        \
        .right    = L'\u2502',                        \
        .tr       = L'\u2510',                        \
        .tl       = L'\u250C',                        \
        .br       = L'\u2518',                        \
        .bl       = L'\u2514',                        \
    }}

/**
 * @brief 
 * Container.
 */
typedef struct agcont_st {

    agobj_t  object;

    agside_t   left;
    agside_t  right;
    agside_t    top;
    agside_t bottom;
    
    agcoord_t length;

    agcontborder_t border;

    struct {

        bool created;
        bool render;

    } flag;

    struct {

        void (*borders) (struct agcont_st*);
        void (*sides)   (struct agcont_st*);
        int  (*display) (struct agcont_st*);
        int  (*all)     (struct agcont_st*);

    } update;

    struct {

        wchar_t  *_1D;
        wchar_t **_2D;

    } display;

    wchar_t *buffer; 
    wchar_t *name;

    agobj_t *parent;
    
    struct {

        uint32_t amount;
        agobj_t   *list;

    } child;

} agcont_t;

/**
 * @brief 
 * Basic window structure.
 */
typedef struct agwin_st {

    uint32_t     width;
    uint32_t    lenght;

    agcoord_t position;

} agwin_t;

/**
 * @brief 
 * Basic terminal cursor info structure.
 */
typedef struct agtermcur_st {
    
    uint32_t      size;
    bool    visibility;

    agcoord_t position;

} agtermcur_t;

typedef struct agterm_st {

    agwin_t     window;
    agtermcur_t cursor;

	uint16_t      rows;
    uint16_t   columns;

    struct {

        bool freeze;
        bool   echo;
        bool minput;

    } flag;

    void (*update)(struct agterm_st*);

} agterm_t;

/**
 * @brief 
 * Refer to "agframe" for more info. 
 */
uint64_t * __agframe_location(void);

/**
 * @brief 
 * Framerate of the program is held in here, you can of course
 * use any variable you want... for now.
 */
#define agframe (*__agframe_location())

/**
 * @fn @c agtermrefresh(0)
 * 
 * @brief ncurses.h refresh() wrapper.
 * Check ncurses refresh() documentation for more info. 
 * 
 * @throw ncurses - Refer to refresh() from ncurses for more info.
 * 
 * @return ncurses ERR upon error or OK if everything went alright
 */
int agtermrefresh(void);

/**
 * @fn @c agtermclear(0)
 * 
 * @brief ncurses.h clear() wrapper.
 * Check ncurses clear() documentation for more info. 
 * 
 * @throw ncurses - Refer to clear() from ncurses for more info.
 * 
 * @return ncurses ERR upon error or OK if everything went alright
 */
int agtermclear(void);

/**
 * @fn @c agtermecho(1)
 * 
 * @brief Calls noecho() if bool echos is == false otherwise calls
 * echo().
 * 
 * @param echos
 *        Boolean that sets the terminal input echo.
 * 
 * @throw ncurses - Refer to noecho() and echo() for more info.
 * 
 * @return ncurses ERR on error, ncurses OK on success.
 */
int agtermecho(bool echos);

/**
 * @fn @c agtermlimits(0)
 * 
 * @brief Return the current minimum and maximum x and y position.
 * 
 * @throw None.
 * 
 * @return Returns a agtermlimit_t structure with the containing
 * x & y min & max. 
 */
agtermlimit_t agtermlimits(void);

/**
 * @fn @c __agtermsizechanged(0)
 * 
 * @brief Refer to agtermsizechanged definition.
 * 
 * @throw None.
 * 
 * @return Boolean indicating whether or not the terminal size changed.
 */
bool __agtermsizechanged(void);

#ifdef AG_PLATFORM_WINDOWS
/**
 * @brief 
 * If the current terminal size changed the value
 * returned by __agtermsizechanged() will be true, every call
 * to the function that occurs while the terminal doesn't change size again
 * will return false.
 * 
 * @warning
 * Platform specific behavior: on Windows resize_term(0,0) will be ran 
 * manually before checking if the size changed to update the COLS and 
 * LINES value of stdscr.
 */
#define agtermsizechanged ({resize_term(0,0); __agtermsizechanged();})
#else
/**
 * @brief 
 * If the current terminal size changed the value
 * returned by __agtermsizechanged() will be true, every call
 * to the function that occurs while the terminal doesn't change size again
 * will return false.
 */
#define agtermsizechanged __agtermsizechanged()
#endif

/**
 * @fn @c agtermcurmove(1)
 * 
 * @brief 
 * Move console cursor to @c coord's position.
 * 
 * @param coord
 *        (x,y) position to move the cursor to.
 * 
 * @throw ncurses - Check what errors the func. move()
 *                  from ncurses.h can return.
 * 
 * @returns Nothing.
 */
int agtermcurmove(
    agcoord_t coord
);

/**
 * @fn @c agtermcurhome(0)
 * 
 * @brief 
 * Move console cursor to (0, 0) position.
 * 
 * @throw ncurses - Check what errors the func. move()
 *                  from ncurses.h can return.
 * 
 * @returns Nothing.
 */
int agtermcurhome(
    void
);

/**
 * @fn @c agtermcurhidden(1)
 * 
 * @brief 
 * Toggle cursor visibility.
 * 
 * @throw Windows - Read agtermcurmove(1) but replace
 *                  SetConsoleCursorPosition with GetCursorInfo
 *                  & SetCursorInfo.
 * @throw Linux - None.
 * 
 * @returns None.
 */
void agtermcurhidden(
    bool hidden
);

/**
 * @fn @c agtermcurpos(0)
 * 
 * @brief
 * Return cursor position.
 * 
 * @throw None.
 * 
 * @return @a agcoord_t structure that contains the x & y positions
 *         of the stdscr cursor.
 */
agcoord_t agtermcurpos(void);

/**
 * @brief 
 * aginit(1) argument structure.
 * See aginit(1) for further description.
 */
typedef struct aginit_arg {

    void (^constructor)(void);
    void (^destructor)(int);

} aginit_arg;

#define AG_CONSTRUCTOR_DEFAULT  \
^void(void) {                   \
    setlocale(LC_ALL, "");      \
	agescseq(ESCSEQ_ALT_BUFFER);\
	initscr();                  \
   	nodelay(stdscr, TRUE);      \
	agtermecho(false);          \
	agtermcurhidden(true);      \
	agtermsizechanged()         \
}

#ifdef AG_DEV
#define AG_DESTRUCTOR_DEFAULT                   \
^void(int _sig) {                               \
agtermecho(true);                               \
    agtermcurhidden(false);                     \
    endwin();                                   \
	agescseq(ESCSEQ_MAIN_BUFFER);               \
    if(__cont != NULL) {                        \
        free(__cont->display._1D);              \
        free(__cont->display._2D);              \
        free(__cont->name);                     \
        free(__cont);                           \
    }                                           \
    switch(_sig) {                              \
        case SIGINT:                            \
        printf("Ctrl-C exit.\n");               \
        break;                                  \
        case SIGSEGV:                           \
        printf("Segfault occurred!\n");         \
        break;                                  \
        default:                                \
        if(_sig != 0)                           \
            printf("Exit signal: %d\n", _sig);  \
        break;                                  \
    }                                           \
    while(viwerr(VIWERR_OCCURED, NULL)){        \
    	viwerr(VIWERR_PRINT, NULL);             \
    }                                           \
}
#else
#define AG_DESTRUCTOR_DEFAULT            \
^void(int _sig) {                        \
    agtermecho(true);                    \
    agtermcurhidden(false);              \
    endwin();                            \
    if(__cont != NULL){                  \
        free(__cont->display._1D);       \
        free(__cont->display._2D);       \
        free(__cont->name);              \
        free(__cont);                    \
    }                                    \
    while(viwerr(VIWERR_OCCURED, NULL)){ \
    	viwerr(VIWERR_PRINT, NULL);      \
    }                                    \
}
#endif

/**
 * @fn @c aginit(1)
 * 
 * @brief 
 * Initialize terminal for use with asigraph.h.
 * 
 * @param aginit
 * 
 *        @tparam @a handler.abnormal
 *        Abnormal program exit handler. Accepts a pointer to a
 *        function that returns nothing and
 *        accepts a integer value. This value is a signal that
 *        can be one of the following values:
 *        SIGABRT, SIGINT, SIGQUIT, SIGTERM, SIGSEGV, SIGILL
 *        & SIGHUP.
 *        SIGILL & SIGSEGV are here because they normally abort
 *        the program.
 *        The function will be executed when any of these signals
 *        are set aka. on abnormal exit.
 *        These functions must use abort() to leave the program
 *        to not call the @a handler.normal function by accident.
 * 
 *        @tparam @a handler.normal
 *        Normal program exit handler. Accepts a pointer to a
 *        function that returns and accepts nothing.
 *        Do not call exit() within the @a handler.normal as it
 *        is undefined behaviour due to @a handler.normal being
 *        passed to atexit().
 *        
 * @throw Windows, Linux - can exit with code AG_EXIT_CODE if 
 *                         the function fails to initialize the terminal.
 * 
 * @returns None.
 */
void aginit(
    aginit_arg arg
);

/**
 * @fn @c __aginitializer_default(0)
 * 
 * @brief 
 * Default initializer that aginit(1) uses.
 * The default initialization is disabling the cursor visibility
 * and terminal echo and changing the buffer to AG_ALT_BUFFER.
 *
 * @throw Windows, Linux - None.
 * 
 * @returns None.
 */
void __aginitializer_default(void);

/**
 * @brief 
 * Arguments for agexhndl, refer to agexhndl(3) for more info on what they do.
 */
enum agexhndl_args_en {
    AGEXHNDL_RUN,
    AGEXHNDL_ADD
};

/**
 * @fn @c agexhndl(3)
 * 
 * @brief 
 * This is where the main block function is stored and then ran during exit,
 * arg accepts values from enum AGEXHNDL_ARGS. Basically atexit() calls
 * __agexhndl_normal() which calls this function and this function calls
 * the passed handler function. We don't need it to be to fast do we?
 * 
 * @param arg
 *        Accepts values from enum agobjtype_en, check the template params.
 *        for more information.
 * 
 * @param sig
 *        Signal from agexhndlsig to handle abnormal exits.
 * 
 * @param handler
 *        Block function for handling abnormal and normal exits.
 * 
 * @tparam arg.AGEXHNDL_RUN
 *         Run the currently stored exit handler, if there is no exit handler
 *         then run the default one
 * 
 * @tparam arg.AGEXHNDL_ADD
 *         Add the currently passed exit handler.
 * 
 * @throw None.
 * 
 * @return Nothing.
 * 
 */
void agexhndl(
    enum agexhndl_args_en arg,
    int signal,
    void (^handler)(int)
);

/**
 * @fn @c agexhndladd
 * 
 * @brief
 * Adds __agexhndl_abnormal to abnormal signal exits & __agexhndl_normal to
 * atexit() function. Not much use outside of that.
 * 
 */
void agexhndladd(
    void (*sigfunc)(int),
    void (*atexitfunc)(void)
);

/**
 * @fn @c __agexhndl_abnormal(1)
 * 
 * @brief The default abnormal exit handler.
 * __agexhndl_abnormal(1) calls exit() within itself, therefore we
 * only use it to return what signal was raised to the normal
 * exit handler aka. __agexhndl_normal(0) by setting agexhndlsig = signal.
 * 
 * @param signal 
 *        Raised exit signal from signal.h
 * 
 */
void __agexhndl_abnormal(int signal);

/**
 * @fn @c __agexhndl_normal(1)
 * 
 * @brief The normal exit handler.
 * Runs agexhndl and exits, this function is used to be given to
 * the atexit() func.
 * 
 * @throw None and must not since its the exit handler.
 * 
 * @returns Does not return at all.
 * 
 */
void __agexhndl_normal(void);

/**
 * @fn @c __agexhndl_signal(0) 
 * 
 * @brief Refer to agexhndlsig macro for more info.
 * 
 */
int * __agexhndl_signal(void);

/**
 * @fn @c agexhndlsig
 * 
 * @brief Used to transfer signals between
 * __agexhndl_abnormal(1) & __agexhndl_normal(0).
 * __agexhndl_abnormal(1) singular argument "sig" is written into
 * agexhndlsig if you want to use the signal in the normal handler.
 * 
 */
#define agexhndlsig (*__agexhndl_signal())

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
 *                  agtermcurmove(1) for what i think on that.
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

/**
 * @fn @c agcontinit(1)
 * 
 * @brief 
 * Initialize a agcont_t container.
 * 
 * @param __container 
 * @return agcont_t* 
 */
agcont_t * agcontinit(
    agcont_t __container
);

/**
 * @fn @c agcontup
 * 
 * @brief 
 * Update a agcont_t.
 * 
 * @param container 
 * @return int 
 */
int agcontup(agcont_t * container);

/**
 * @brief 
 * For testing purposes.
 */
__attribute__((deprecated))
void agcontdraw(
    agcont_t * c, 
    agcoord_t pos
);

#ifdef AG_DEV
agcont_t ** __agcont(void);
#define __cont (*__agcont())
#endif

#endif /** @c ASIGRAPH_LIBRARY_INCLUDED */ 
