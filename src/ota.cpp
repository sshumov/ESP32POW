
#include "main.h"
#include "ota.h"
#ifdef USE_OTA
Task OTA_task(0, TASK_FOREVER, &ota_task_callback);

void ota_task_callback(void) {
    ArduinoOTA.handle();
}

void init_ota(Scheduler *sc) {
  #ifdef DEBUG
    print_DEBUG("OTA: start OTA task");
  #endif
    ArduinoOTA.onStart([]() {
        String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
        type = "sketch";
    } else { // U_FS
        type = "filesystem";
    }
    // NOTE: if updating FS this would be the place to unmount FS using FS.end()
     print_DEBUG("Start updating " + type);
    });

    ArduinoOTA.onEnd([]() {
#ifdef DEBUG
        print_DEBUG("End");
#endif
    });
    ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
#ifdef DEBUG
        DBG_OUTPUT_PORT.printf("Progress: %u%%\r", (progress / (total / 100)));
#endif
    });
    ArduinoOTA.onError([](ota_error_t error) {
#ifdef DEBUG
     print_DEBUG("Error[%u]: " + error);
    if (error == OTA_AUTH_ERROR) {
      print_DEBUG("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      print_DEBUG("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      print_DEBUG("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      print_DEBUG("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      print_DEBUG("End Failed");
    }
#endif
    });
    sc->addTask(OTA_task);
    OTA_task.enable();
    ArduinoOTA.begin(); 
}
#endif
