
#include <ESPmDNS.h>

#include <TaskScheduler.h>
#include "WiFiSettings.h"

#include <FTPServer.h>
#include <LittleFS.h>

#include "main.h"
#include "buttom.h"
#include "lua.h"

#ifdef  USE_OTA
#include "ota.h"
#endif

Scheduler        SCHEDULER;                  // Системный шедулер

DS3231           RTCClock;
RTClib           RTC;
DateTime         DTM;

//FtpServer        ftpSrv;
FTPServer ftpSrv(SPIFFS); // construct with LittleFS

Preferences      PREF;
DNSServer        dns;
WebServer        Wserver(80);
bool             wifi_status,safe_mode;

WiFiSettingsClass WiFiSettings(&Wserver,&dns);

void wifi_task_callback(void);

Task WIFI_task(0, TASK_FOREVER, &wifi_task_callback);

int system(const char *cmd) {
  return 1;
}

void print_DEBUG(String msg) {
  #ifdef DEBUG
      DBG_OUTPUT_PORT.println(msg);
  #endif
}

String slurp(const String& fn) {
  return PREF.getString(fn.c_str());
}

void spurt(const String& fn, const String& content) {
  PREF.putString(fn.c_str(),content);
}

void WiFiEvent(WiFiEvent_t event)
{
  switch (event) {
    case SYSTEM_EVENT_STA_DISCONNECTED:
      print_DEBUG("Disconnected from WiFi access point");
      wifi_status = false;
      yield();
      WiFi.reconnect();
      digitalWrite(LED_GREEN,HIGH);
      break;
    case SYSTEM_EVENT_STA_GOT_IP:
      print_DEBUG("Obtained IP address: ");
      wifi_status = true;
      digitalWrite(LED_BLUE,LOW);
      break;
    default: break;
    }
}

void wifi_task_callback(void) {
  if(WIFI_task.isFirstIteration()) {
    //ftpSrv.begin("admin",WiFiSettings.password.c_str());
    ftpSrv.begin("admin",WiFiSettings.password.c_str());
    
  }

  if (WiFi.status() != WL_CONNECTED) {
      wifi_status = false;
  } else {
    ftpSrv.handleFTP();
  }
}


void init_wifi(Scheduler *sc) {
  print_DEBUG("Start wifi ...");
    if (WiFi.getAutoReconnect() != true)    //configuration will be saved into SDK flash area
           WiFi.setAutoReconnect(true);    //automatically reconnects to hwAP in case it's disconnected

  WiFi.onEvent(WiFiEvent);
  WiFiSettings.AP = false;
  WiFiSettings.connect(false, 5);
  WiFiSettings.portal();
  if (MDNS.begin(WiFiSettings.hostname.c_str())) 
    { MDNS.addService("http", "tcp", 80); }

  sc->addTask(WIFI_task);
  WIFI_task.enable();

}




void setup() {
  
  DBG_OUTPUT_PORT.begin(115200);
  print_DEBUG("Starting ...");
  PREF.begin(APP_NAME, false);
  safe_mode = PREF.getBool("safe_mode",false);

  if(safe_mode == true ) {
    print_DEBUG("ENTER SAFE MODE");
    PREF.putBool("safe_mode",false);
    digitalWrite(LED_RED,HIGH);
    WiFiSettings.AP = true;
    WiFiSettings.portal();
  }
  //SPIFFS.format();
  SPIFFS.begin(true);
  

  SCHEDULER.init();
  init_buttom(&SCHEDULER);
  init_wifi(&SCHEDULER);
  init_lua(&SCHEDULER);
}

void loop() {
  SCHEDULER.execute();
  delay(1);
}
