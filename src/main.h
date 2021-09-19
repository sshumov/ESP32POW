#define APP_NAME "TOP-323"

#include <Arduino.h>
#include <WIFi.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include "WiFiSettings.h"
#include <Wire.h>
#include "DS3231.h"


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


#define     TH1                   32
#define     TH2                   39
#define     TH3                   36
#define     LED_GREEN             26             // Номер GPIO  використовуємий для ввімкненя зеленого діоду
#define     LED_RED               25             // Номер GPIO  використовуємий для ввімкненя червоного діоду
//#define     LED_BLUE              33             // Номер GPIO  використовуємий для ввімкненя блакитного діоду
#define     LED_BLUE              2             // Номер GPIO  використовуємий для ввімкненя блакитного діоду
#define     R1                    04
#define     R2                    02
#define     R3                    17
#define     R4                    16
#define     R5                    12
#define     R6                    13
#define     CP                    27             // Номер GPIO ESP32 використовуємий для зміни стану реле (тригер)
#define     CS_1                  05
#define     BUTTON_PIN            0              // Номер GPIO ESP32 використовуємий для кнопки
#define     DELAY_BUTTON_ACTION1  80             // Затримка нажатої кномки для виконання дії 1 зазначено у мс
#define     DELAY_BUTTON_ACTION2  5000           // Затримка нажатої кномки для виконання дії 2 зазначено у мс
#define     READ                  0b00000001     //This value tells  that data is to be read from the requested register.
#define     WRITE                 0b00000000     //This value tells  that data is to be written to the requested register.

#define Gain      0xE60F // (R/W) Default: 0x0000, Unsigned, PGA gains at ADC inputs. (16 bit)
#define Run       0xE228 // (R/W) Default: 0x0000, Unsigned, The run register starts and stops the DSP (see the Digital Signal Processor section). (16 bit)

#define AIRMS     0x43C0 // (R), Default: N/A, Signed, Phase A current rms value. (24 bit, 32 ZP - sign extended to 32 bit)
#define AVRMS     0x43C1 // (R), Default: N/A, Signed, Phase A voltage rms value. (24 bit, 32 ZP - sign extended to 32 bit)
#define BIRMS     0x43C2 // (R), Default: N/A, Signed, Phase B current rms value. (24 bit, 32 ZP - sign extended to 32 bit)
#define BVRMS     0x43C3 // (R), Default: N/A, Signed, Phase B voltage rms value. (24 bit, 32 ZP - sign extended to 32 bit)
#define CIRMS     0x43C4 // (R), Default: N/A, Signed, Phase C current rms value. (24 bit, 32 ZP - sign extended to 32 bit)
#define CVRMS     0x43C5 // (R), Default: N/A, Signed, Phase C voltage rms value. (24 bit, 32 ZP - sign extended to 32 bit)
#define AWATT     0xE513 // (R), Default: N/A, Signed, (24 bit, 32 SE - sign extended to 32 bit), Instantaneous value of Phase A total active power.
#define BWATT     0xE514 // (R), Default: N/A, Signed, (24 bit, 32 SE - sign extended to 32 bit), Instantaneous value of Phase B total active power.
#define CWATT     0xE515 // (R), Default: N/A, Signed, (24 bit, 32 SE - sign extended to 32 bit), Instantaneous value of Phase C total active power.

extern Preferences      PREF;
extern WebServer        Wserver;

// WIFI status
extern bool wifi_status;

extern DS3231 RTCClock;
extern RTClib RTC;
extern DateTime DTM;

void print_DEBUG(String msg);
String slurp(const String& fn);
void spurt(const String& fn, const String& content);
int system(const char *cmd);
