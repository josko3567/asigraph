#include <stdio.h>
#include "../asigraph.h"
#include "../ext/viwerr/viwerr.h"

int main(void) {
	
	viwerr(VIWERR_PUSH, &(viwerr_package){
		.code = 5,
		.name = (char*)"Hello",
		.group = (char*)" world",
		.message = (char*)"!!!"
	});
	errno = 100;
	viwerr(VIWERR_PRINT, NULL);
	viwerr(VIWERR_PRINT, NULL);
	viwerr(VIWERR_PRINT, NULL);
	viwerr(VIWERR_PRINT, NULL);
        agframe += 100;
        printf("%d\n", agframe);
        return 0;

}