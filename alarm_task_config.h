#ifndef C_ALARM__
#define C_ALARM__
#include "config.h"
extern struct task_queue ALARM_QUEUE;

#define ALARM_REPORT(x, y, z, f, e) taskQueuePush(&ALARM_QUEUE, x, y, z, f, e)

#endif
