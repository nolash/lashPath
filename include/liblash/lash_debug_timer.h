#ifndef LASH_DEBUG_TIMER_H_
#define LASH_DEBUG_TIMER_H_

#include <time.h>

typedef struct lash_debug_timer_t {
	unsigned int count;
	long total;
	struct timespec snap_start;
	struct timespec snap_end;
	struct timespec snap_tmp;
	int running;
	int paused;
} lash_debug_timer_t;

#ifdef __cplusplus 
extern "C" {
#endif
void lash_debugTimerReset(lash_debug_timer_t *time);
void lash_debugTimerStart(lash_debug_timer_t *time);
void lash_debugTimerStop(lash_debug_timer_t *time);
long lash_debugTimerPause(lash_debug_timer_t *time);
float lash_debugTimerGetAverage(lash_debug_timer_t *time);

#ifdef __cplusplus 
}
#endif

#endif
