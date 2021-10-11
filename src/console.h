#ifndef _CONSOLE_
#define _CONSOLE_
#include "RemoteDebug.h"  
#include "RemoteDebugger.h"		//https://github.com/JoaoLopesF/RemoteDebugger

#endif
extern RemoteDebug Debug;
void init_console(Scheduler *sc);
void console_task_callback(void);