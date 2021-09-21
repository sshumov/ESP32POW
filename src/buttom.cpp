#include "main.h"
#include "buttom.h"

Task BUTTOM_task(0, TASK_FOREVER, &buttom_task_callback);

uint32_t  timeButtonDown;
uint8_t   xPressButton_action;


#define SHORT_PRESS_TIME 500  // 500 milliseconds
#define LONG_PRESS_TIME  2000 // 2s long press


// Variables will change:
static u_int lastState = LOW;  // the previous state from the input pin
static u_int currentState;     // the current reading from the input pin

static u_long pressedTime  = 0;
static u_long releasedTime = 0;


void buttom_task_callback() {
  currentState = digitalRead(BUTTON_PIN);
  if(lastState == HIGH && currentState == LOW)        // button is pressed
    pressedTime = millis();
  else if(lastState == LOW && currentState == HIGH) { // button is released
    releasedTime = millis();

  long pressDuration = releasedTime - pressedTime;
  
  if(pressDuration > LONG_PRESS_TIME ) {
  // Enter safe mode
    print_DEBUG("press long");
    PREF.putBool("safe_mode",true);
    yield();
    ESP.restart();
  }

  if(pressDuration > SHORT_PRESS_TIME && pressDuration < LONG_PRESS_TIME ) {
    print_DEBUG("Long press");
  }

  if( pressDuration < SHORT_PRESS_TIME ) {
    print_DEBUG("Short press");
  }
 }
 lastState = currentState;
}





void init_buttom(Scheduler *sc) {
  print_DEBUG("Buttom init ...");
  #ifdef HALL_BUTTOM
    print_DEBUG("Enable hall effect sensor. Bring the magnet to the chip to enter safe mode.");
  #endif
  pinMode(BUTTON_PIN, INPUT);
  lastState = digitalRead(BUTTON_PIN);

  sc->addTask(BUTTOM_task);
  BUTTOM_task.enable();
}


