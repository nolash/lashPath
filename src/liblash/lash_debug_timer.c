#include <time.h>

#include "liblash/lash_debug_timer.h"

void lash_debugTimerReset(lash_debug_timer_t *time) {
	time->running = 0;
	time->paused = 0;
	time->count = 0;
	time->total = 0;
}

void lash_debugTimerStart(lash_debug_timer_t *time) {
	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &(time->snap_start));
	time->running = 1;
}

long lash_debugTimerPause(lash_debug_timer_t *time) {
	if (!time->running)
		return 0;
		
	if (time->paused) {
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &(time->snap_tmp));
		time->snap_start.tv_nsec += time->snap_tmp.tv_nsec - time->snap_end.tv_nsec;
		time->paused = 0;
		return time->snap_tmp.tv_nsec;
	} else {
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &(time->snap_end));
		time->paused = 1;
		return time->snap_end.tv_nsec;
	}
}

void lash_debugTimerStop(lash_debug_timer_t * time) {
	if (time->running == 0)
		return;
	
	if (!time->paused)
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &(time->snap_end));
		
	time->total = time->snap_end.tv_nsec - time->snap_start.tv_nsec;
	time->count++;
	time->running = 0;
	time->paused = 0;
}

float lash_debugTimerGetAverage(lash_debug_timer_t *time) {
	return time->total / (float)time->count;
}
