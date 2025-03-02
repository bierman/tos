#include <pigpio.h>
#include <stdio.h>

#include "tos-io.h"


int main(void) {
  // Initialize pigpio and check for failure
  if (gpioInitialise() < 0) {
    printf("pigpio initialization failed\n");
    return 1;
  }

  // Set the LED pin as output
  gpioSetMode(TOS_IO_LED_RED, PI_OUTPUT);
  gpioSetMode(TOS_IO_LED_GREEN, PI_OUTPUT);
  gpioSetMode(TOS_IO_LED_BLUE, PI_OUTPUT);

  // Turn off all LEDs initially
  gpioWrite(TOS_IO_LED_RED, 0);
  gpioWrite(TOS_IO_LED_GREEN, 0);
  gpioWrite(TOS_IO_LED_BLUE, 0);

  while (1) {
    // Turn the Red LED on
    gpioWrite(TOS_IO_LED_RED, 1);
    gpioDelay(1000000); // Wait for 1 second
    // Turn the Red LED off
    gpioWrite(TOS_IO_LED_RED, 0);
    gpioDelay(1000000); // Wait for 1 second
    
    // Turn the Green LED on
    gpioWrite(TOS_IO_LED_GREEN, 1);
    gpioDelay(1000000);
    // Turn the Green LED off
    gpioWrite(TOS_IO_LED_GREEN, 0);
    gpioDelay(1000000);
  
    // Turn the Blue LED on
    gpioWrite(TOS_IO_LED_BLUE, 1);
    gpioDelay(1000000);
    // Turn the Blue LED off
    gpioWrite(TOS_IO_LED_BLUE, 0);
    gpioDelay(1000000);
  }

  gpioTerminate();
  return 0;
}