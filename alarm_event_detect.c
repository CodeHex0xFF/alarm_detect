#include "alarm_event_detect.h"
#include "alarm_task_config.h"

int32_t mem_detect(struct alarm_report *rep) {
  if (rep != 0) {
  }
  return 0;
}

struct alarm_event mem_alarm = {.nice = 1,
                                .alarmName = "MEM ABNORMAL",
                                .maxThrehold = 20,
                                .minThrehold = 10};

struct alarm_report mem_alarm_report = {
    .event = &mem_alarm,
};

REGISTER_ALARM_EVENT(hello, &mem_alarm_report, mem_detect);
