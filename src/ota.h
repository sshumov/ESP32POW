#ifndef _OTA_
#define _OTA_
#include "main.h"
#ifdef USE_OTA
#include <ArduinoOTA.h>

void ota_task_callback(void);

void init_ota(Scheduler *sc);
#endif
#endif
