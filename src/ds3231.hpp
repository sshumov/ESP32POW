#ifndef _DS3231_
#define _DS3231_
#include <TaskSchedulerDeclarations.h>
#include "DS3231.h"

void init_ds3231(Scheduler *sc);
void ds3231_task_callback(void);
#endif
