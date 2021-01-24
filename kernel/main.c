#include "nos/monitor.h"

void
kmain(void)
{
  monitor_clear();
  monitor_write("Hello nos kernel!");
}
