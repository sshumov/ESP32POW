#include "main.h"
#include "buttom.h"

Task BUTTOM_task(5, TASK_FOREVER, &buttom_task_callback);

uint32_t  timeButtonDown;
uint8_t   xPressButton_action;

void IRAM_ATTR pressButtonInterrupt() {
  volatile bool level = digitalRead(BUTTON_PIN);
  if (!level) {
    timeButtonDown = millis();
  }
  if (level) {
    volatile uint32_t dif = millis() - timeButtonDown;
    if ((dif >= DELAY_BUTTON_ACTION1) && (dif < DELAY_BUTTON_ACTION1 * 10)) {
         xPressButton_action = PRESS_SHORT;
         timeButtonDown = 0;
    }
    if ((dif >= DELAY_BUTTON_ACTION2) && (dif < DELAY_BUTTON_ACTION2 * 2)) {
           xPressButton_action = PRESS_LONG;
           timeButtonDown = 0; }
  } 
}

void buttom_task_callback() {
  int hall = 0;
  hall = hallRead();
  #ifdef HALL_BUTTOM
    if(hall >100 ) xPressButton_action = PRESS_LONG;
  #endif
    if (xPressButton_action == PRESS_SHORT) {
        xPressButton_action=PRESS_NONE;
        print_DEBUG("press short");
   } else if(xPressButton_action == PRESS_LONG) {
        xPressButton_action=PRESS_NONE;
        print_DEBUG("press long");
        PREF.putBool("safe_mode",true);
        yield();
        ESP.restart();
  }
}


void init_buttom(Scheduler *sc) {
  timeButtonDown = xPressButton_action = 0;
  print_DEBUG("Buttom init ...");
  #ifdef HALL_BUTTOM
    print_DEBUG("Enable hall effect sensor. Bring the magnet to the chip to enter safe mode.");
  #endif
  pinMode(BUTTON_PIN, INPUT);
  attachInterrupt(BUTTON_PIN, pressButtonInterrupt, CHANGE);
  sc->addTask(BUTTOM_task);
  BUTTOM_task.enable();
}
