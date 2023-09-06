#include <stdio.h>
#include <stdlib.h>
#include <locale.h>
#include "../asigraph.h"

void __aginitializer_default(
	void)
{
	setlocale(LC_ALL, "");
	initscr();
    	nodelay(stdscr, TRUE);
	agtermecho(false);
	agtermcurhidden(true);
	return;
}

/**
 * @brief 
 * Setup for the terminal.
 */
void aginit(
	aginit_arg arg) 
{

	// Initialize handler for all types of exits.
	agexhndladd(
		arg.handler.abnormal != NULL ?
			arg.handler.abnormal : __agexhndl_abnormal,
		arg.handler.normal != NULL ? 
			arg.handler.normal : __agexhndl_normal
	);
	
	// Run the initializer-
	arg.initializer = arg.initializer != NULL ? 
		arg.initializer : __aginitializer_default;
	arg.initializer();
	return;

}
