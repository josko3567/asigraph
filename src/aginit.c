#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "../asigraph.h"

/**
 * @brief 
 * Setup for the terminal.
 */
void aginit(
	aginit_arg arg) 
{

	/* Input new exit handler. */
	agexhndl(
		AGEXHNDL_ADD,
		0,
		arg.destructor == NULL ? 
			AG_DESTRUCTOR_DEFAULT : arg.destructor
	); 

	/* Initialize "middleman" handlers for all 
	types of exits. */
	agexhndladd(
		__agexhndl_abnormal,
		__agexhndl_normal
	);
	
	// Run the initializer.
	arg.constructor = (arg.constructor == NULL) ?
		AG_CONSTRUCTOR_DEFAULT : arg.constructor;
	arg.constructor();

}
