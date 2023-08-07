#ifndef __ALARM_TASK_EVENT__
#define __ALARM_TASK_EVENT__

#include "alarm_task_config.h"
#include "config.h"

struct alarm_report {
  struct alarm_event *event;
  void *args;
};

struct alarm_reflect {
  struct alarm_report *report_data;
  int32_t (*report)(struct alarm_report *);
};

#define REGISTER_ALARM_EVENT(__name, __event, __report)                        \
  __attribute__((                                                              \
      section("alarm_event"),                                                  \
      aligned(__alignof__(                                                     \
          struct alarm_reflect)))) struct alarm_reflect _setup##__name = {     \
      .report_data = __event, .report = __report}

extern struct alarm_reflect __start_alarm_event;
extern struct alarm_reflect __stop_alarm_event;

#endif
