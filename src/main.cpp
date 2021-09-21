
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

// ESP32 get memory stat
void mem_stat(void) {
  // total_free_bytes;      ///<  Total free bytes in the heap. Equivalent to multi_free_heap_size().
  // total_allocated_bytes; ///<  Total bytes allocated to data in the heap.
  // largest_free_block;    ///<  Size of largest free block in the heap. This is the largest malloc-able size.
  // minimum_free_bytes;    ///<  Lifetime minimum free heap size. Equivalent to multi_minimum_free_heap_size().
  // allocated_blocks;      ///<  Number of (variable size) blocks allocated in the heap.
  // free_blocks;           ///<  Number of (variable size) free blocks in the heap.
  // total_blocks;          ///<  Total number of (variable size) blocks in the heap.
  multi_heap_info_t info;
  uint32_t free;uint16_t max;uint8_t frag;
  heap_caps_get_info(&info, MALLOC_CAP_INTERNAL);
  free = info.total_free_bytes;
  max  = info.largest_free_block;
  frag = 100 - (max * 100) / free;
  #ifdef DEBUG
    DBG_OUTPUT_PORT.printf("[MEM] free: %5d | max: %5d | frag: %3d%% \n", free, max, frag);
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
      //digitalWrite(LED_GREEN,HIGH);
      break;
    case SYSTEM_EVENT_STA_GOT_IP:
      print_DEBUG("Obtained IP address: ");
      wifi_status = true;
      //digitalWrite(LED_BLUE,LOW);
      break;
    default: break;
    }
}

void wifi_task_callback(void) {
  if(WIFI_task.isFirstIteration()) {
    ftpSrv.begin("admin",WiFiSettings.password.c_str());
  }
  if (WiFi.status() != WL_CONNECTED) {
      wifi_status = false;
  } else {
    ftpSrv.handleFTP();
  }
}


void init_wifi(Scheduler *sc) {
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
    //digitalWrite(LED_RED,HIGH);
    WiFiSettings.AP = true;
    WiFiSettings.portal();
  }
  Wire.begin();
  SPIFFS.begin(true);
  

  SCHEDULER.init();
  init_buttom(&SCHEDULER);
  init_wifi(&SCHEDULER);
  #ifdef USE_OTA
    init_ota(&SCHEDULER);
  #endif

  init_lua(&SCHEDULER);
}

void loop() {
  SCHEDULER.execute();
  delay(1);
}
