#ifndef _NOS_MONITOR_H_
#define _NOS_MONITOR_H_

// Write a single character out to the screen.
void monitor_put(char c);

// Clear the screen to all black.
void monitor_clear();

// Output a null-terminated ASCII string to the monitor.
void monitor_write(char *c);

#endif // _NOS_MONITOR_H_
