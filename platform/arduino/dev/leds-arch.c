#include <dev/leds.h>
#include "Arduino.h"

#define YELLOW_PIN 13

void
leds_arch_init(void)
{
  pinMode(YELLOW_PIN, OUTPUT);
}

unsigned char
leds_arch_get(void)
{
  unsigned char on = 0;
  
  if (digitalRead(YELLOW_PIN)) on |= LEDS_YELLOW;

  return on;
}

void
leds_arch_set(unsigned char leds)
{
  digitalWrite(YELLOW_PIN, leds & LEDS_YELLOW);
}
