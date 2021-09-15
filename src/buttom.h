#ifndef _BUTTOM_
#define _BUTTOM_
#define PRESS_NONE   0
#define PRESS_SHORT  1
#define PRESS_LONG   2
#include "main.h"

#include <TaskSchedulerDeclarations.h>

void buttom_task_callback(void);
void init_buttom(Scheduler *sc);
extern uint32_t  timeButtonDown;
extern uint8_t   xPressButton_action;
#endif
