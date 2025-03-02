#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdint.h>

#include "tos-io.h"


#define GPIO_BASE 0xFE200000
#define BLOCK_SIZE 4096

#define GPIO_IN(g)  (*(gpio+((g)/10)) &= ~(7<<(((g)%10)*3)))
#define GPIO_OUT(g) (*(gpio+((g)/10)) |=  (1<<(((g)%10)*3)))
#define GPIO_SET(g) (*(gpio+7) = 1 << (g))
#define GPIO_CLR(g) (*(gpio+10) = 1 << (g))
#define GPIO_READ(g) (*(gpio+13) & (1 << (g)))

volatile unsigned *gpio;

void setup_io() {
  int mem_fd;
  void *gpio_map;

  if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
    perror("open");
    exit(EXIT_FAILURE);
  }

  gpio_map = mmap(
    NULL,
    BLOCK_SIZE,
    PROT_READ | PROT_WRITE,
    MAP_SHARED,
    mem_fd,
    GPIO_BASE
  );

  close(mem_fd);

  if (gpio_map == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }

  gpio = (volatile unsigned *) gpio_map;
}

int main(int argc, char** argv) {
  setup_io();

  GPIO_IN(TOS_IO_BUTTON);
  GPIO_OUT(TOS_IO_LED_RED);

  while (1) {
    if (!(GPIO_READ(TOS_IO_BUTTON))) { // Button pressed (active low)
      GPIO_SET(TOS_IO_LED_RED); // Turn on LED
    } else {
      GPIO_CLR(TOS_IO_LED_RED); // Turn off LED
    }
    usleep(10000);
  }

  return 0;
}