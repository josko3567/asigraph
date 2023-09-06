#include "../ext/viwerr/viwerr.h"
#include "../asigraph.h"

#if defined(_WIN32) || defined(_WIN64)

#include <stdbool.h>
/* QueryPerformanceCounter & QueryPerformaceFrequency */
#include <profileapi.h>

static __inline__ int asigraph_precise_sleep_win( 
	const double sleeptime );
static __inline__ double asigraph_timer_win_get_framerate(
	LARGE_INTEGER freq,
	LARGE_INTEGER ct,
	LARGE_INTEGER lt );
static __inline__ double asigraph_timer_win_get_deltatime(
	LARGE_INTEGER freq,
	LARGE_INTEGER ct,
	LARGE_INTEGER lt );
static __inline__ int asigraph_timer_win( 
	const double target_framerate, 
	double *framerate,
	double *deltatime );

/*
 * Get deltatime from frequency, current time and last time
 * returns 0.0 if ct.QuadPart - lt.QuadPart <= 0 
 */
static __inline__ double asigraph_timer_win_get_deltatime(
	LARGE_INTEGER freq,
	LARGE_INTEGER ct,
	LARGE_INTEGER lt )
{

	return 
		(ct.QuadPart - lt.QuadPart) > 0 ?
			(double)(ct.QuadPart - lt.QuadPart) 
			/ (double)freq.QuadPart
		:
			0.000;

}

/*
 * Get framerate from frequency, current time and last time
 * returns 0.0 if ct.QuadPart - lt.QuadPart <= 0 
 */
static __inline__ double asigraph_timer_win_get_framerate(
	LARGE_INTEGER freq,
	LARGE_INTEGER ct,
	LARGE_INTEGER lt )
{

	return 
		(ct.QuadPart - lt.QuadPart) > 0 ? 
			(double)freq.QuadPart 
			/ (double)(ct.QuadPart - lt.QuadPart)
		:
			0.000;

}

/*
 * Windows machine precision sleep
 * with QueryPerformanceCounter.
 */
static __inline__ int asigraph_precise_sleep_win( 
	const double sleeptime )
{

	LARGE_INTEGER freq, start, time;
	QueryPerformanceFrequency(&freq);
	QueryPerformanceCounter(&start);
	
	while( 1 ){
		
		QueryPerformanceCounter( &time );
		if( asigraph_timer_win_get_deltatime( 
		freq, time, start ) > sleeptime )
			break;
		
	}
	return 0;

}

/* 
 * Precise sleep function defined for windows.
 */

int agsleep( 
	const double sleeptime )
{

	if( sleeptime < 0.000 ){

		viwerr(VIWERR_PUSH, &(viwerr_package){
			.code = EINVAL,
			.group = (char*)"errno"
		});
		return -1;

	}

	return asigraph_precise_sleep_win( sleeptime );

}

/*
 * Windows framerate counter using
 * QueryPerformanceCounter.
 * Inserting 0.0 to target_framerate 
 * sets the cap to infinite.
 */
static __inline__ int asigraph_timer_win( 
	const double target_framerate, 
	double *framerate, 
	double *deltatime )
{

	static bool startup = true;
	static LARGE_INTEGER lt, ct; /* Last time and current time */
	static LARGE_INTEGER freq; /* Freq of timer */
	double curfr; /* Current framerate */
	double curdt; /* Current delta time */
	
	if( startup ){
		
		QueryPerformanceFrequency( &freq );
		QueryPerformanceCounter( &ct );
		startup = false;
		goto exit_true;

	}

	/* write current time to last time and update current time */
	lt = ct;
	QueryPerformanceCounter( &ct );
	curfr = asigraph_timer_win_get_framerate( 
		freq, ct, lt );
	curdt = asigraph_timer_win_get_deltatime( 
		freq, ct, lt );

	/* No framerate cap */
	if( target_framerate == 0.000 ){

		QueryPerformanceCounter( &ct );
		curfr = asigraph_timer_win_get_framerate( 
			freq, ct, lt );
		curdt = asigraph_timer_win_get_deltatime( 
			freq, ct, lt );
		if( framerate ) *framerate = curfr;
		if( deltatime ) *deltatime = curdt;
		goto exit_true;

	}
	
	/* Sleep through the remainder of the frame */
	if( target_framerate < curfr ){
		
		double sleeptime = 1.0/target_framerate - curdt;
		agsleep( sleeptime < 0.000 ? 0.000 : sleeptime );

		QueryPerformanceCounter( &ct );
		curfr = asigraph_timer_win_get_framerate( 
			freq, ct, lt );
		curdt = asigraph_timer_win_get_deltatime( 
			freq, ct, lt );
		
	} 

	if( framerate ) *framerate = curfr;
	if( deltatime ) *deltatime = curdt;

/* Exit success */
exit_true:
	return 0;

}


/*
 * Timer function.
 * Place in a while loop like this:
 * while( agtimer( 60.0, &framerate, &deltatime ) ){
 * 	
 * 	...
 * 
 * }
 * target_framerate of 0.0 will set the cap to infinite
 * Returnes true if no erros, false if errors
 * Returns framerate and deltatime to *framerate
 * and *deltatime unless they are NULL.
 */
int agtimer( 
	const double target_framerate, 
	double *framerate, 
	double *deltatime ){

	if( target_framerate < 0.000 ) {

		viwerr(VIWERR_PUSH, &(viwerr_package){
			.code = EINVAL,
			.group = (char*)"errno"
		})
		return -1;

	}

	return asigraph_timer_win( 
		target_framerate, 
		framerate, 
		deltatime 
	);

}
#else

/*
 * time.h breaks with -std=c...
 * declare _XOPEN_SOURCE to be 600
 * to fix compilation.
 */
#if __STDC_VERSION__ >= 199901L
	#define _XOPEN_SOURCE 600
#else
	#define _XOPEN_SOURCE 500
#endif

#include <stdbool.h>
#include <time.h> /* timespec clock_gettime & nanosleep */
#include <errno.h> /* EINTR & EINVAL */

#define NS_IN_S_LL ((long long)1000000000)
#define NS_IN_S_LF ((double)1000000000.0)
#define CLOCK      CLOCK_MONOTONIC
/*
 * Conversion from double sleep time in sec
 * to timespec sec and nsec.
 */
static __inline__ struct timespec agsleep_ts_conv( 
	double sleeptime )
{

	struct timespec sleeptime_ts = {
		.tv_nsec = 0,
		.tv_sec  = 0
	};

	if( sleeptime > 0.0 ) {
	
		sleeptime_ts.tv_sec = (time_t)sleeptime;
		sleeptime_ts.tv_nsec = 
			(long)((sleeptime - (double)((long)sleeptime)) 
			* NS_IN_S_LL);

	}
	return sleeptime_ts;

}

/*
 * Test function
 * Precise sleep for linux machine
 * now given in timespec
 */
static __inline__ 
int agsleep_ts( 
	struct timespec ts_sleep_time )
{

	struct timespec remenant = {
		.tv_sec = 0,
		.tv_nsec = 0
	};

	while(1) {
		
		int ret = nanosleep( &ts_sleep_time, &remenant );
		if( ret == -1 && errno == EINTR ) {
			/* Interruption occured, write remenant into 
			   ts_sleep_time and continue */
			ts_sleep_time.tv_sec = remenant.tv_sec;
			ts_sleep_time.tv_nsec = remenant.tv_nsec;
			viwerr(VIWERR_POP, &(viwerr_package){
				.group = (char*)"errno"
			});
			errno = 0;
			continue; 

		} else if( errno != 0 ){
			
			/* Push errno to viwerr and continue. */
			viwerr(VIWERR_OCCURED, NULL);
			break;

		} else {

			break;

		}

	}
	return 0;

}

/* 
 * Precise sleep function defined for linux.
 */

int agsleep( 
	const double sleeptime )
{

	if( sleeptime < 0.000 ){

		viwerr(VIWERR_PUSH, &(viwerr_package){
			.code = EINVAL,
			.group = (char*)"errno"
		});
		return -1;

	}

	return agsleep_ts(agsleep_ts_conv(sleeptime));

}

int agtimer( 
	const double target_framerate, 
	double *retfr, 
	double *retdt ) 
{

	if( target_framerate < 0.000 ) {

		viwerr(VIWERR_PUSH, &(viwerr_package){
			.code = EINVAL,
			.group = (char*)"errno"
		});
		return -1;

	}

	static struct {
		struct timespec cur;
		struct timespec prev;
	} time;

	static bool start = true;

	time.prev.tv_nsec = time.cur.tv_nsec;
	time.prev.tv_sec  = time.cur.tv_sec;
	clock_gettime(CLOCK, &time.cur);

	if(start) {	
		start = false;
		return 0;
	}

	long long elapsed = 
		(time.cur.tv_sec * 1000000000LL + time.cur.tv_nsec)
		- (time.prev.tv_sec * 1000000000LL + time.prev.tv_nsec);

	long long sleeptime_ns = 
		(long long)(((double)(1.000)/target_framerate)
		* (double)1E9) - elapsed;

	// Clamp to 0
	sleeptime_ns = (sleeptime_ns < 0 ? 0 : sleeptime_ns);

	struct timespec sleeptime = {
		.tv_nsec = (target_framerate == 0.000) ? 
			0 :
			(long)(long long)(sleeptime_ns%1000000000LL),
		.tv_sec = (target_framerate == 0.000) ? 
			0 :
			(time_t)
			(sleeptime_ns - (long long)(sleeptime_ns%1000000000LL))
	};

	if(!(sleeptime.tv_nsec == 0 && sleeptime.tv_sec == 0)) {

		agsleep_ts(sleeptime);

	}

	clock_gettime(CLOCK, &time.cur);
	elapsed = 
		(time.cur.tv_sec * 1000000000LL + time.cur.tv_nsec)
		- (time.prev.tv_sec * 1000000000LL + time.prev.tv_nsec);

	if(retfr) (*retfr) = (1E9 / (double)elapsed);
	if(retdt) (*retdt) = ((double)elapsed / 1E9);

	return 0;

}

#endif
