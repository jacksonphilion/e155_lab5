/*

// DEBUG.c

Jackson Philion, Oct.24.2024, jphilion@g.hmc.edu

This code is derived from Kavi Dey, shared to E155 students in Fall 2024
to support the printf function in main.c, used to print my RPM values to
the users screen. A link to his shared tutorial can be found here: 
https://kavidey.notion.site/Debugging-in-Segger-with-printf-114a56ecd5de8045a9ffe1b0a805db4d
*/

#include "DEBUG.h"

// Function used by printf to send characters to the laptop
int _write(int file, char *ptr, int len) {
  int i = 0;
  for (i = 0; i < len; i++) {
    ITM_SendChar((*ptr++));
  }
  return len;
}

