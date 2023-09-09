#include <stdio.h>
#include <stdbool.h>
#include <limits.h>
// #include <unistd.h>
#include "../asigraph.h"
#include "../ext/viwerr/viwerr.h"

/**
 * @brief 
 * Update the sides of the container.
 * 
 * @param container 
 */
void agcontsideup(
        agcont_t * container) 
{

    if(!container->flag.created) return;

    container->left.__prepos   = container->left.__curpos;
    container->right.__prepos  = container->right.__curpos;
    container->top.__prepos    = container->top.__curpos;
    container->bottom.__prepos = container->bottom.__curpos;

    agtermlimit_t tlim = agtermlimits();

    if(container->left.priority  == 1 
    && container->right.priority == 0 ){
            
        container->right.__curpos = container->right.position(
            0, tlim.x.max
        );
        container->left.__curpos = container->left.position(
            container->right.__curpos, tlim.x.max
        );

        if(container->right.__curpos <= container->left.__curpos ) {
            // printw("aaaaaaaa");    
            container->right.__curpos =
                container->left.__curpos+1;
        }

    } else {

        container->left.__curpos = container->left.position(
            0, tlim.x.max
        );
        container->right.__curpos = container->right.position(
            container->left.__curpos, tlim.x.max
        );

        if(container->right.__curpos <= container->left.__curpos ) {
                
            container->right.__curpos =
                container->left.__curpos+1;
        }

    }

    if(container->top.priority    == 1 
    && container->bottom.priority == 0 ){
            
        container->bottom.__curpos = container->bottom.position(
            0, tlim.y.max
        );
        container->top.__curpos = container->top.position(
            container->bottom.__curpos, tlim.y.max
        );

        if(container->bottom.__curpos <= container->top.__curpos ) {
                
            container->bottom.__curpos =
                container->top.__curpos+1;
        }

    } else {

        container->top.__curpos = container->top.position(
            0, tlim.y.max
        );
        container->bottom.__curpos = container->bottom.position(
            container->top.__curpos, tlim.y.max
        );

        if(container->bottom.__curpos <= container->top.__curpos ) {
                
            container->bottom.__curpos =
                container->top.__curpos+1;
        }

    }

    container->length.x = AG_CONTAINER_X_LENGTH(container);
    container->length.y = AG_CONTAINER_Y_LENGTH(container);

    return;

}

/**
 * @brief 
 * Normalize a container passed as a parameter to agcontinit();
 * 
 * @param container  
 * @return agcont_t* 
 */
agcont_t * agcontnormalize(
        agcont_t c )
{

    if( c.left.position   == NULL
    ||  c.right.position  == NULL
    ||  c.top.position    == NULL
    ||  c.bottom.position == NULL
    ) {

        viwerr(VIWERR_PUSH, &(viwerr_package){
            .code = EINVAL,
            .name =
            (char*)"agcontnormalize(1): Invalid parameter.",
            .message =
            (char*)"container.<side>.position cannot be "
            "NULL. A block function is required.",
            .group = AG_ERROR_GROUP
        });

        return NULL;

    }

    if( c.name == NULL ) {

        viwerr(VIWERR_PUSH, &(viwerr_package){
            .code = EINVAL,
            .name =
            (char*)"agcontnormalize(1): Invalid parameter.",
            .message =
            (char*)"container.name cannot be "
            "NULL. A string is required.",
            .group = AG_ERROR_GROUP
        });

        return NULL;

    }

    /**
     * @brief
     * Here we test wether or not one of the sides is dependent on
     * the other. We test the function with a list of random numbers that
     * are a mix of primes and random numbers from small to big.
     * If with any of the numbers from the list the x value deviates,
     * the function is set to be dependent on the other side.
     * An error is thrown if both sides are dependent on one another.
     * Of course there is a chance this will not catch a dependant side,
     * but its probably a very small chance.
     */

    bool   
		leftdep = false,
      	rightdep = false,
        topdep = false,
    	bottomdep = false;

    // A mix of primes and regular numbers.
    int primesmix[] = {
             2,  3,  5,  8, 11, // Mix
            13, 17, 20, 23, 29,
            31, 37, 41, 45, 47,
            53, 55, 61, 71, 76,
            73, 79, 83, 90, 97,
            512, 9455, 97614, 659273, 8239767, //Not prime
            271, 2753, 42457, 973177, 9063781 // Prime
    };

    // Left side check
    for(uint32_t i = 0, x = c.left.position(primesmix[0], UINT32_MAX);
        i < sizeof(primesmix)/sizeof(int);
        ++i)
    {
        if(x != c.left.position(primesmix[i], UINT32_MAX)) {
            leftdep = true;
            break;
        }
    }

    // Right side check
    for(uint32_t i = 0, x = c.right.position(primesmix[0], UINT32_MAX);
        i < sizeof(primesmix)/sizeof(int);
        ++i)
    {
        if(x != c.right.position(primesmix[i], UINT32_MAX)) {
            rightdep = true;
            break;
        }
    }

    // Top side check
    for(uint32_t i = 0, x = c.top.position(primesmix[0], UINT32_MAX);
        i < sizeof(primesmix)/sizeof(int);
        ++i)
    {
        if(x != c.top.position(primesmix[i], UINT32_MAX)) {
            topdep = true;
            break;
        }
    }

    // Bottom side check
    for(uint32_t i = 0, x = c.bottom.position(primesmix[0], UINT32_MAX);
        i < sizeof(primesmix)/sizeof(int);
        ++i)
    {
        if(x != c.bottom.position(primesmix[i], UINT32_MAX)) {
            bottomdep = true;
            break;
        }
    }

    // Error check...
    if( leftdep == true && rightdep == true ) {

        viwerr(VIWERR_PUSH, &(viwerr_package){
            .code = EINVAL,
            .name =
            (char*)"agcontnormalize(1): Invalid parameter.",
            .message =
            (char*)"container.left.position() and "
            "container.right.position() both depend on "
            "each other. Only one side can depend on "
            "another side.",
            .group = AG_ERROR_GROUP
        });

        return NULL;

    }

    if( topdep == true && bottomdep == true ) {

        viwerr(VIWERR_PUSH, &(viwerr_package){
            .code = EINVAL,
            .name =
            (char*)"agcontnormalize(1): Invalid parameter.",
            .message =
            (char*)"container.top.position() and "
            "container.bottom.position() both depend on "
            "each other. Only one side can depend on "
            "another side.",
            .group = AG_ERROR_GROUP
        });

        return NULL;

    }

    agcont_t * _cont = (agcont_t*)malloc(sizeof(agcont_t));

    if(leftdep == true) {
        _cont->left.priority = 1;
    } else {
        _cont->left.priority = 0;
    }

    if(rightdep == true) {
        _cont->right.priority = 1;
    } else {
        _cont->right.priority = 0;
    }

    if(bottomdep == true) {
        _cont->bottom.priority = 1;
    } else {
        _cont->bottom.priority = 0;
    }

    if(topdep == true) {
        _cont->top.priority = 1;
    } else {
        _cont->top.priority = 0;
    }

    return _cont;
}

void agcontbrdup(
        agcont_t * container) 
{

    if(container->border.draw  == false
    || container->flag.created == false)
        return;
            
    agcoord_t len = (agcoord_t){
        .x = container->length.x,
        .y = container->length.y
    };

    // if(len.x < 0 && )
    if(len.x > 0 && len.y > 0){
        container->display._2D[0][0]             = container->border.icon.tl;
        container->display._2D[0][len.x-1]       = container->border.icon.tr;
        container->display._2D[len.y-1][0]       = container->border.icon.bl;
        container->display._2D[len.y-1][len.x-1] = container->border.icon.br;
    }

    for(int32_t i = 1; i < len.x-1; i++) 
        container->display._2D[0][i]       = container->border.icon.top;
    
    for(int32_t i = 1; i < len.x-1; i++) 
        container->display._2D[len.y-1][i] = container->border.icon.bottom;
    
    for(int32_t i = 1; i < len.y-1; i++)
        container->display._2D[i][0]       = container->border.icon.left;

    for(int32_t i = 1; i < len.y-1; i++)
        container->display._2D[i][len.x-1] = container->border.icon.right;

    

}

int agcontdisup(
    agcont_t * c )
{

    c->display._1D = realloc(
        c->display._1D, sizeof(wchar_t*)*(AG_CONTAINER_DISPLAY_SIZE(c)+1)
    );

    if(c->display._1D == NULL) {
        viwerr(VIWERR_PUSH, &(viwerr_package){
            .code    = ENOMEM,
            .name    = (char*)"agcontdisup() failed...",
            .message = (char*)"ENOMEM: Could not allocate the"
            " requested amount of memory.",
            .group   = AG_ERROR_GROUP
        });
        return ENOMEM;
    }

    c->display._1D[AG_CONTAINER_DISPLAY_SIZE(c)] = (wchar_t)0;

    c->display._2D = realloc(
        c->display._2D, sizeof(wchar_t*)*(AG_CONTAINER_Y_LENGTH(c) < 2 ?
            2 : AG_CONTAINER_Y_LENGTH(c))
    );

    if(c->display._2D == NULL) {
            viwerr(VIWERR_PUSH, &(viwerr_package){
            .code    = ENOMEM,
            .name    = (char*)"agcontdisup() failed...",
            .message = (char*)"ENOMEM: Could not allocate the"
            " requested amount of memory.",
            .group   = AG_ERROR_GROUP
        });
        return ENOMEM;
    }

    for(uint32_t i = 0; i < AG_CONTAINER_Y_LENGTH(c); i++) {
        c->display._2D[i] = 
            (wchar_t*)((c->display._1D)+(i*AG_CONTAINER_X_LENGTH(c)));
    }

	wmemset(c->display._1D, (wchar_t)' ',
		AG_CONTAINER_DISPLAY_SIZE(c)
	);

    return 0;

}


/**
 * @brief 
 * Create a new container object.
 * 
 * @param arg Paramaters.
 * 
 * @return agcont_t* 
 */
agcont_t * agcontinit(
        agcont_t arg)
{

    static uint64_t uqid = 0;

    agcont_t * container = agcontnormalize(arg);
    if( container == NULL ) {

        viwerr(VIWERR_PUSH, &(viwerr_package){
            .code = EINVAL,
            .name =
            (char*)"agcontinit(1): Invalid parameter.",
            .message =
            (char*)"agcontnormalize(1) failed to initialize "
            "a container. More info in error message from "
            "agcontnormalize(1).",
            .group = AG_ERROR_GROUP
        });

        return NULL;

    } 
    
    // Initalize object info.
    container->object.id.type = AG_OBJECT_CONTAINER.id.type;
    container->object.id.unique = uqid++;
    container->object.name = (char*)AG_OBJECT_CONTAINER.name;
    container->object.description = (char*)AG_OBJECT_CONTAINER.description;

    // Copy over values from arg.
    container->left.position = arg.left.position;
    container->left.padding  = arg.left.padding;
    container->left.__curpos = 0;
    container->left.__prepos = 0;

    container->right.position = arg.right.position;
    container->right.padding  = arg.right.padding;
    container->right.__curpos = 0;
    container->right.__prepos = 0;

    container->bottom.position = arg.bottom.position;
    container->bottom.padding  = arg.bottom.padding;
    container->bottom.__curpos = 0;
    container->bottom.__prepos = 0;

    container->top.position = arg.top.position;
    container->top.padding  = arg.top.padding;
    container->top.__curpos = 0;
    container->top.__prepos = 0;

    container->flag.created = true;
    container->flag.render  = arg.flag.render;

    container->border.draw  = true;

    container->border.icon.bottom = arg.border.icon.bottom;
    container->border.icon.top    = arg.border.icon.top;
    container->border.icon.left   = arg.border.icon.left;
    container->border.icon.right  = arg.border.icon.right;

    container->border.icon.bl = arg.border.icon.bl;
    container->border.icon.br = arg.border.icon.br;
    container->border.icon.tl = arg.border.icon.tl;
    container->border.icon.tr = arg.border.icon.tr;

    container->update.borders = agcontbrdup;
    container->update.sides = agcontsideup;
    container->update.display = agcontdisup;
    container->update.all = agcontup;

    container->display._1D = NULL;
    container->display._2D = NULL;

    // container->child = NULL;
    // container->

    // Allocate space for name, copy it and write it to container.
    wchar_t * name = (wchar_t*)calloc(
        sizeof(wchar_t), AG_CONTAINER_NAME_SIZE+1
    );

    if(name == NULL) {

        viwerr(VIWERR_PUSH, &(viwerr_package){
            .code = ENOMEM,
            .name = (char*)"agcontinit(1): Memory exhausted.",
            .message = (char*)"Whilst the function was running, "
            "malloc() or calloc() returned a NULL value.",
            .group = AG_ERROR_GROUP
        });

        free(container);
        return NULL;

    }

#ifdef AG_PLATFORM_IS_WINDOWS
    	wcsncpy_s( name, AG_CONTAINER_NAME_SIZE+1, arg.name, AG_CONTAINER_NAME_SIZE );
#else
	wcsncpy( name, arg.name, AG_CONTAINER_NAME_SIZE );
#endif

    container->name = name;

    // Update position twice so that __prevpos isn't 0 after one update.
    agcontsideup(container);
    agcontsideup(container);

    // Allocate space for display in a 1D format.
    agcontdisup(container);

    // Draw borders if plausible.
    agcontbrdup(container);
        
    return container;

}

int agcontup(
    agcont_t * container )
{

    if(container->flag.created == false) {
        viwerr(VIWERR_PUSH, &(viwerr_package){
            .code = EINVAL,
            .name =
            (char*)"agcontup(1): Invalid parameter.",
            .message =
            (char*)"agcontup(1) was given a container "
            "that was not first initalized with agcontinit(1).",
            .group = AG_ERROR_GROUP
        });
        return EINVAL;
    }

    agcontsideup(container);

    if(agcontdisup(container) != 0) {
        viwerr(VIWERR_PUSH, &(viwerr_package){
            .code = ENOMEM,
            .name =
            (char*)"agcontup(1) failed...",
            .message =
            (char*)"ENOMEM: function agcontdisup(1) failed with "
            "ENOMEM, check the error message from agcontdisup() "
            "for more info. ",
            .group = AG_ERROR_GROUP
        });
        return ENOMEM;
    }

    agcontbrdup(container);

    return 0;

}

__attribute__((deprecated))
void agcontdraw(agcont_t * ct, agcoord_t pos) {

    if(ct->flag.created == false
    || ct->display._1D  == NULL
    || ct->display._2D  == NULL ) {
        viwerr(VIWERR_PUSH, &(viwerr_package){
            .code = EINVAL,
            .name = (char*)"agcontdraw() failed...",
            .message = (char*)"agcontdraw(1) was given a container "
            "that was not first initalized with agcontinit(1).",
            .group = AG_ERROR_GROUP
        });  
    }

    agtermlimit_t tlim = agtermlimits();
    wchar_t **display = ct->display._2D;
    int32_t ly = ct->length.y;

    if(!(pos.x > (int32_t)tlim.x.max || pos.y > (int32_t)tlim.y.max)
    && ct->flag.render == true) {
        for(int32_t
            y   = pos.y < 0 ? 0 : pos.y,     // Printing position y, incremented
            x   = pos.x < 0 ? 0 : pos.x,     // Printing position x, static
            y0  = pos.y < 0 ? 0 - pos.y : 0, // Starting string, incremented
            x0  = pos.x < 0 ? 0 - pos.x : 0, // Starting string pos, static
            len =                            // Length of printed part/whole str
                (pos.x+ct->length.x < (int32_t)tlim.x.max ? 
                    pos.x+ct->length.x : (int32_t)tlim.x.max)
                - (pos.x < (int32_t)tlim.x.min ? 
                    (int32_t)tlim.x.min : pos.x)
        ; y0 < ly
        ; y0++, y++)
        {
            mvaddnwstr(
                y, x,
                (display[y0]+x0),
                len
            );
        }
    }                                          
                                                  
}

agcont_t ** __agcont(void) {
    static agcont_t * cont = NULL;
    return &cont;
}
