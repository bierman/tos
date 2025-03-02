#include <pigpio.h>
#include <pthread.h>
#include <stdio.h>
#include <stdbool.h>
#include "font.h"
#include <signal.h>

#include "tos-io.h"


static volatile bool running = true;


#define PIN_COL_0 TOS_IO_PMOD_IO2
#define PIN_COL_1 TOS_IO_PMOD_IO3
#define PIN_COL_2 TOS_IO_PMOD_IO4
#define PIN_ROW_0 TOS_IO_PMOD_IO6
#define PIN_ROW_1 TOS_IO_PMOD_IO7
#define PIN_ROW_2 TOS_IO_PMOD_IO8

static const int matrix[] = {PIN_COL_0, PIN_COL_1, PIN_COL_2, PIN_ROW_0, PIN_ROW_1, PIN_ROW_2};


static volatile uint64_t pattern = 0;

static int list_on[64] = {0};
static int last_on = 0;
static int current_on = 0;

static void* update(void* arg) {
  uint64_t last_pattern = 0;

  while (running) {
    if (pattern != last_pattern) {
      last_pattern = pattern;
      last_on = 0;
      current_on = 0;
      uint64_t p = pattern;

      // Look for LEDs that are on
      for (int i = 0; i < 64; i++) {
        bool on = p & 1;
        if (on) {
          list_on[last_on++] = i;
        }
        p >>= 1;
      }
    }

    for (int i = 0; i < last_on; i++) {
      int x = list_on[i];
      for (int k = 0; k < 6; k++) {
        gpioWrite(matrix[k], (x >> k) & 1);
      }
      gpioDelay(100);
    }
  }
  return NULL;
}

void handle_signal(int signal) {
  if (signal == SIGINT) {
    running = false;
  }
}

void ticker_tape(const char* msg) {
  const char* current_char = msg;
  uint64_t current_pattern = 0;
  uint64_t next_pattern = 0;
  while(running) {
    if (*current_char == '\0') {
      current_char = msg;
    }
    next_pattern = 0;
    for (int i = 0; i < 8; i++) {
      next_pattern <<= 8;
      next_pattern |= console_font_6x8[*current_char * 8 + i];
    }
    current_char++;
    for (int i = 0; i < 6; i++) {
      current_pattern <<= 1;
      current_pattern &= ~0x0101010101010101;
      for (int j = 0; j < 8; j++) {
        if (next_pattern & ((uint64_t) 0x80 << (j * 8))) {
          current_pattern |= (uint64_t) 1 << (j * 8);
        }
      }
      next_pattern <<= 1;
      pattern = current_pattern;
      gpioDelay(60000);
    }
  }
}

int main(void) {
  // Initialize pigpio and check for failure
  if (gpioInitialise() < 0) {
    printf("pigpio initialization failed\n");
    return 1;
  }

  // Set the LED pin as output
  for (int i = 0; i < 6; i++) {
    gpioSetMode(matrix[i], PI_OUTPUT);
  }
  signal(SIGINT, handle_signal);

  pthread_t thread_id;
  if (pthread_create(&thread_id, NULL, update, NULL) != 0) {
    printf("Failed to create thread\n");
    return 1;
  }

  ticker_tape("Hello, TOS!!! ");
  
  pthread_join(thread_id, NULL);

  gpioTerminate();
  return 0;
}