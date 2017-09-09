#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <math.h>
//------------------------------------------------------------------------------------------------
#define DISPLAY_WIDTH 1920
#define DISPLAY_HEIGHT 1080
#define C_DENOM 1024
#define PIXELS_STEP 25
#define T_DELAY 400000000
#define GREY_SENSETIVE 12
//------------------------------------------------------------------------------------------------
Display *display;
struct timespec tr;
struct timespec tw = {0,T_DELAY};
//------------------------------------------------------------------------------------------------
XColor main_color(XImage *image);
