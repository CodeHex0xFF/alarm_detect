#include "alarm_task_config.h"
#include "alarm_event_detect.h"
#include "config.h"

struct task_queue ALARM_QUEUE;

void *alarm_task_queue(void *args) {
  struct task_queue *alarm = (struct task_queue *)args;
  if (alarm == 0) {
    return 0;
  }
  setupTaskQueue(alarm);
  taskQueueDeal(alarm);
  return 0;
}

int alarm_task_run(pthread_t *tid) {
  return pthread_create(tid, 0, alarm_task_queue, &ALARM_QUEUE);
}

void *detect_alarm_run(void *args) {
  while (true) {
    sleep(1);
    for (struct alarm_reflect *pos = &__start_alarm_event;
         pos < &__stop_alarm_event; pos++) {
      if (pos->report != 0) {
        pos->report(pos->report_data);
      }
    }
  }
  return NULL;
}

int main(void) {
  pthread_t tid;
  pthread_create(&tid, 0, detect_alarm_run, 0);
  alarm_task_queue(&tid);
  pthread_join(tid, 0);
  return 0;
}
