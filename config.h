#pragma once
#include "list.h"
#include <assert.h>
#include <errno.h>
#include <pthread.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/shm.h>
#include <time.h>
#include <unistd.h>

#define TASK_BUSY BIT(0)
#define TASK_EMPTY BIT(1)
#define TASK_ALIVE BIT(2)
#define TASK_KILLED BIT(3)

#define THREAD_ENABLE BIT(0)
#define MUTEX_ENABLE BIT(1)
#define COND_ENABLE BIT(2)
#define ATTR_ENABLE BIT(3)

#define UDS_CLI_TASK BIT(0)
#define UDS_NETWORK_TASK BIT(1)
#define UDS_SQL_CHECK BIT(2)

#define DETECT_TASK BIT(3)

struct timeout {
  bool enable;
  time_t start;
  time_t stop;
  time_t time_out;
};

struct alarm_reference {
  uint32_t error_counts;
  uint32_t recover_counts;
};

struct alarm_event {
  bool enable;
  const char *alarmName;
  struct timeout alarm_timeout;
  struct alarm_reference refer;
  uint32_t nice;
  void *args;
  uint32_t minThrehold;
  uint32_t maxThrehold;
  int32_t (*alarm_call)(void *args);
};

// TODO:
//  may be create thread
//  just for hardware watch serice
//  all task based queue deal;
//  dymaic destroy lock
struct task {
  bool enable;
  bool thread_enable;
  bool mutex_enable;
  bool cond_enable;
  bool attr_enable;
  uint32_t used_cnts;
  uint32_t task_status;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  pthread_attr_t attr;
  pthread_t tid;
  const char *name;
  void *args;
  union {
    // thread using
    void *(*exec)(void *);
    // not thread using
    int32_t (*exec_once)(void *);
  };
};

struct task_queue {
  // TODO: Mutex from other thread
  clock_t start;
  char taskName[32];
  pthread_mutex_t mutex;
  pthread_cond_t cond;
  bool empty;
  bool busy;
  void *task_args;
  void *deal_callback_args;
  int32_t (*deal_callback)(void *);
  int32_t (*task)(void *, int32_t (*deal_callback)(void *), void *);
  clock_t end;
  struct LIST_NODE node;
};

static inline int32_t setupTaskQueue(struct task_queue *task) {
  if (task == 0) {
    return -2;
  }
  task->empty = true;
  task->start = 0;
  task->end = 1;
  task->node.next = &task->node;
  task->node.prev = &task->node;
  pthread_mutex_init(&task->mutex, 0);
  pthread_cond_init(&task->cond, 0);
  return 0;
}

// NOTE: add mutex lock
static inline int32_t taskQueuePush(
    struct task_queue *task, const char *taskName, void *task_args,
    void *deal_callback_args,
    int32_t (*task_cb)(void *, int32_t (*deal_callback)(void *), void *),
    int32_t (*deal_callback)(void *)) {
  if (task == 0) {
    return -1;
  }
  usleep(abs((int)(task->end - task->start)) * 100);
  struct task_queue *new_task = calloc(sizeof(struct task_queue), 1);
  if (new_task == 0) {
    return -3;
  }
  task->empty = false;
  bzero(new_task->taskName, sizeof(new_task->taskName));
  strcpy(new_task->taskName, taskName);
  new_task->task_args = task_args;
  new_task->deal_callback_args = deal_callback_args;
  new_task->task = task_cb;
  list_add(&new_task->node, &task->node);
  pthread_mutex_lock(&task->mutex);
  pthread_cond_broadcast(&task->cond);
  pthread_mutex_unlock(&task->mutex);
  return 0;
}

static inline int32_t taskQueueDeal(struct task_queue *task) {
  if (task == 0) {
    return -1;
  }
  struct task_queue *cur;
  struct LIST_NODE *node;
  while (true) {
    while (task->empty) {
      pthread_mutex_lock(&task->mutex);
      pthread_cond_wait(&task->cond, &task->mutex);
      pthread_mutex_unlock(&task->mutex);
    }
    node = task->node.next;
    while (node != &task->node) {
      cur = container_of(node, struct task_queue, node);
      if (cur != 0) {
        task->start = clock();
        if (cur->task != 0) {
          cur->task(cur->task_args, cur->deal_callback,
                    cur->deal_callback_args);
        }
        task->end = clock();
      }
      list_del(node);
      node = node->next;
      free(cur);
      cur = 0;
    }
    task->empty = true;
  }
  return 0;
}

static inline int32_t isTaskQueueEmpty(struct task_queue *task) {
  if (task == 0) {
    return -1;
  }
  return task->empty;
}

static inline __attribute__((__unused__)) char *runtime(void) {
  struct timespec times = {0, 0};
  static char time[128];
  clock_gettime(CLOCK_MONOTONIC, &times);
  sprintf(time, "%lu.%lu", times.tv_sec, times.tv_nsec);
  return time;
}
