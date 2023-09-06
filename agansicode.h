#ifndef AGANSICODE_HEADER_INCLUDED
#define AGANSICODE_HEADER_INCLUDED

#define agescseq printf

// void * __agescseq(str, ...) {

//         if(str == NULL) {
//                 viwerr(VIWERR_PUSH, &(viwerr_package){
//                         .code = EINVAL,
//                         .group = AG_ERROR_GROUP,
//                         .name = (char*)"__agescseq(?): invalid "
//                         "parameter \"str\".",
//                         .message = (char*)"__agescseq(?)"
//                 });
//         }

// }

/**
 * @brief 
 * Move back to the secondary screen buffer.
 */
#define ESCSEQ_ALT_BUFFER    "\x1b[?1049h"

/**
 * @brief 
 * Move back to the main screen buffer.
 */
#define ESCSEQ_MAIN_BUFFER   "\x1b[?1049l"

/**
 * @brief 
 * Move the cursor to (0,0)
 */
#define ESCSEQ_CURSOR_HOME        "\x1b[H"

/**
 * @brief 
 * Move cursor x lines down.
 */
#define ESCSEQ_CURSOR_MOVE(x,y)   "\x1b[%d;%dH", x, y

/**
 * @brief 
 * Move cursor x lines up.
 */
#define ESCSEQ_CURSOR_UP(x)       "\x1b[%dA", x

/**
 * @brief 
 * Move cursor x lines down.
 */
#define ESCSEQ_CURSOR_DOWN(x)     "\x1b[%dB", x

/**
 * @brief 
 * Move cursor x columns right.
 */
#define ESCSEQ_CURSOR_RIGHT(x)    "\x1b[%dC", x

/**
 * @brief 
 * Move cursor x columns lef.
 */
#define ESCSEQ_CURSOR_LEFT(x)     "\x1b[%dD", x

/**
 * @brief 
 * Move the cursor to the beginning of the line that
 * is x lines down.
 */
#define ESCSEQ_CURSOR_DOWN_BOL(x) "\x1b[%dE", x

/**
 * @brief 
 * Move the cursor to the beginning of the line that
 * is x lines up.
 */
#define ESCSEQ_CURSOR_UP_BOL(x)   "\x1b[%dF", x

/**
 * @brief 
 * Move the cursor to the x-th column.
 */
#define ESCSEQ_CURSOR_COLUMN(x)   "\x1b[%dG", x

/**
 * @brief 
 * Request cursor postion readable from read(stdin, ...);
 */
#define ESCSEQ_REQUEST_CURSOR_POSITION "\x1b[6n"

#endif /** @c AGANSICODE_HEADER_INCLUDED */
