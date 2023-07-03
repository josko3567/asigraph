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

		// matica_stderr.lin.set( EINVAL );
		errno = EINVAL;
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

		// matica_stderr.lin.set( EINVAL );
		errno = EINVAL;
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

#include "../ext/viwerr/viwerr.h"
#include <stdbool.h>
#include <time.h> /* timespec clock_gettime & nanosleep */
#include <errno.h> /* EINTR & EINVAL */

#define ASIGRAPH_TIMER_LINUX_NS_REF_LL ((long long)1000000000)
#define ASIGRAPH_TIMER_LINUX_NS_REF_LF ((double)1000000000.0)

/* Constants ASIGRAPH_TIMER_LINUX_NS_REF_LL
and ASIGRAPH_TIMER_LINUX_NS_REF_LF */

static __inline__ struct timespec asigraph_precise_sleep_linux_conv( 
	double sleeptime );
static __inline__ int asigraph_precise_sleep_linux( 
	const double sleeptime );
static __inline__ int asigraph_timer_linux_update_time( 
	struct timespec * __restrict ct, 
	struct timespec * __restrict lt );
static __inline__ long long asigraph_timer_linux_elapsed_ns( 
	struct timespec const ct, 
	struct timespec const lt );
static __inline__ double    asigraph_timer_linux_getfr_ns( 
	const long long elapsed );
static __inline__ double    asigraph_timer_linux_getdt_ns( 
	const long long elapsed );
static __inline__ int      asigraph_timer_linux( 
	const double target_framerate, 
	double *framerate, 
	double *deltatime );

/*
 * Conversion from double sleep time in sec
 * to timespec sec and nsec.
 */
static __inline__ struct timespec asigraph_precise_sleep_linux_conv( 
	double sleeptime )
{

	struct timespec ts_buf;
		ts_buf.tv_sec = 0;
		ts_buf.tv_nsec = 0;
	if( sleeptime > 0.0 && sleeptime != 0.0 ) {
	
		ts_buf.tv_sec = (long)sleeptime;
		ts_buf.tv_nsec = 
			(long)((sleeptime - (double)((long)sleeptime)) 
			* ASIGRAPH_TIMER_LINUX_NS_REF_LF);

	}
	return ts_buf;

}

/*
 * Test function
 * Precise sleep for linux machine
 * now given in timespec
 */
static __inline__ bool asigraph_precise_sleep_linux_ts( 
	struct timespec ts_sleep_time )
{

	struct timespec ts_remain;
		ts_remain.tv_sec = 0;
		ts_remain.tv_nsec = 0;

	while( 1 ) {
		
		int ret = nanosleep( &ts_sleep_time, &ts_remain );
		if( ret == -1 && errno == EINTR ) {
			/* Interruption occured, write ts_remain into 
			   ts_sleep_time and continue */
			ts_sleep_time.tv_sec = ts_remain.tv_sec;
			ts_sleep_time.tv_nsec = ts_remain.tv_nsec;
			continue; 

		} else {

			break;

		}

	}
	return 0;

}

/*
 * Linux machine precision sleep
 * with nanosleep.
 */
static __inline__ bool asigraph_precise_sleep_linux( 
	const double sleeptime )
{

	struct timespec ts_sleep_time 
		= asigraph_precise_sleep_linux_conv( sleeptime );
	struct timespec ts_remain;
		ts_remain.tv_sec = 0;
		ts_remain.tv_nsec = 0;

	while( 1 ) {
		
		int ret = nanosleep( &ts_sleep_time, &ts_remain );
		if( ret == -1 && errno == EINTR ) {
			/* Interruption occured, write ts_remain into 
			   ts_sleep_time and continue */
			ts_sleep_time.tv_sec = ts_remain.tv_sec;
			ts_sleep_time.tv_nsec = ts_remain.tv_nsec;
			continue; 

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

		// matica_stderr.lin.set( EINVAL );
		errno = EINVAL;
		return -1;

	}

	return asigraph_precise_sleep_linux( sleeptime );

}

/*
 * Write timespec struct ct into lt and update ct.
 * Always returns 0
 */
int asigraph_timer_linux_update_time( 
	struct timespec * __restrict ct, 
	struct timespec * __restrict lt )
{

	lt->tv_sec  = ct->tv_sec;
	lt->tv_nsec = ct->tv_nsec;
	return clock_gettime( CLOCK_MONOTONIC, ct );
	
}

/*
 * Get elapsed time.
 * Returns sum of ct in nanoseconds - sum of lt in nanoseconds.
 */

static __inline__ long long asigraph_timer_linux_elapsed_ns( 
	struct timespec const ct, 
	struct timespec const lt )
{

	long long ct_ns = (long long)ct.tv_nsec + 
		(long long)ct.tv_sec * ASIGRAPH_TIMER_LINUX_NS_REF_LL;
	long long lt_ns = (long long)lt.tv_nsec + 
		(long long)lt.tv_sec * ASIGRAPH_TIMER_LINUX_NS_REF_LL;
	/* long long can store a max 9223372036854775807,
	   which is ((((9223372036854775807 / 10^9)/60s)/60min)/24h)/365d
	   = 292.47 years in nanoseconds,
	   overflow won't be a problem */
	return ct_ns - lt_ns;
	
}

/*
 * Get framerate in Hz from elapsed in ns.
 */
static __inline__ double asigraph_timer_linux_getfr_ns( 
	const long long elapsed )
{

	return elapsed > 0 ? 
		ASIGRAPH_TIMER_LINUX_NS_REF_LF / (double)elapsed : 0.0;

}

/*
 * Get deltatime in s from elapsed in ns.
 */
static __inline__ double asigraph_timer_linux_getdt_ns( 
	const long long elapsed )
{

	return elapsed > 0 ? 
		(double)elapsed / ASIGRAPH_TIMER_LINUX_NS_REF_LF : 0.0;

}

/*
 * Linux timer function,
 * Returns 1 upon
 * success, otherwise 0
 * Optional returns framerate and delatime.
 */
static __inline__ bool asigraph_timer_linux( 
	const double target_framerate, 
	double *framerate, 
	double *deltatime )
{

	double curfr;
	double curdt;
	long long elapsed;
	static struct timespec ct;
	static struct timespec lt; /* Current time and last time */
	static bool startup = true;

	/* Get current time on first call and leave */
	if( startup ){

		clock_gettime( CLOCK_MONOTONIC, &ct );
		startup = false;
		goto exit_true;
		
	}

	asigraph_timer_linux_update_time( &ct, &lt );
	elapsed = asigraph_timer_linux_elapsed_ns( ct, lt );
	curfr = asigraph_timer_linux_getfr_ns( elapsed );
	curdt = asigraph_timer_linux_getdt_ns( elapsed );

	/* No framerate cap */
	if( target_framerate == 0.0 ){

		/* write fr and dt */
		if( framerate ) *framerate = curfr;
		if( deltatime ) *deltatime = curdt;
		goto exit_true;
		
	} else if( target_framerate < curfr ){
		
		double sleeptime = 1.0/target_framerate - curdt;
		agsleep( sleeptime < 0.000 ? 0.000 : sleeptime )
		clock_gettime( CLOCK_MONOTONIC, &ct )
		elapsed = asigraph_timer_linux_elapsed_ns( ct, lt );
		curfr = asigraph_timer_linux_getfr_ns( elapsed );
		curdt = asigraph_timer_linux_getdt_ns( elapsed );
		
	}

	if( framerate ) *framerate = curfr;
	if( deltatime ) *deltatime = curdt;

exit_true:
	return true;

}

/*
 * Timer function defined for linux.
 */

int agtimer( 
	const double target_framerate, 
	double *framerate, 
	double *deltatime )
{


	if( target_framerate < 0.000 ) {

		// matica_stderr.lin.set( EINVAL );
		errno = EINVAL;
		return -1;

	}
	// target_framerate+=1.0;
	return asigraph_timer_linux( 
		target_framerate, framerate, deltatime 
	);

}

#endif