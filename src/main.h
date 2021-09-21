#define APP_NAME "TOP-323"

#include <Arduino.h>
#include <WIFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include "WiFiSettings.h"
#include <Wire.h>

#include "lua.h"

#define USE_LittleFS

#ifdef USE_LittleFS
  #define SPIFFS LITTLEFS
  #include <LITTLEFS.h> 
#else
  #include <SPIFFS.h>
#endif 


#include <FS.h>
#include <TaskSchedulerDeclarations.h>
#include <Preferences.h>


#define DBG_OUTPUT_PORT Serial
#define FATAL(func, msg) { DBG_OUTPUT_PORT.print("Fatal: " func " "); DBG_OUTPUT_PORT.println(msg);}


extern Preferences      PREF;
extern WebServer        Wserver;

// WIFI status
extern bool wifi_status;

void mem_stat(void);
void print_DEBUG(String msg);
String slurp(const String& fn);
void spurt(const String& fn, const String& content);
int system(const char *cmd);
