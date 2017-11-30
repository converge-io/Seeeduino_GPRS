#include "mbed.h"
#include "gprs.h"
#include "SEGGER_RTT.h"

GPRS gprs;

char addr[] = "\"+4407565804622\"";
char msg[] = "Hello World!";

// main() runs in its own thread in the OS
int main() {
  SEGGER_RTT_printf(0,"Starting SIM800...\n");
  gprs.preInit();

  SEGGER_RTT_printf(0,"Initializing GPRS...\n");
  while(0 != gprs.init()) {
      wait(1);
      SEGGER_RTT_printf(0,"init error\r\n");
  }
  SEGGER_RTT_printf(0,"Init success, start to send SMS message...");
  if(gprs.sendSMS("07565804622","hello,world") == 0){
    SEGGER_RTT_printf(0,"Send successful!");
  } //define phone number and text
  else{
    SEGGER_RTT_printf(0,"Send failed!");
  }
  while (true) { }
}
