#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
//------------------------------------------------------------------------------------------------
#define DISPLAY_WIDTH 1920
#define DISPLAY_HEIGHT 1080
#define C_DENOM 1024
#define PIXELS_STEP 12
#define T_DELAY 800000000
#define T_SLEEP 1
#define GREY_SENSETIVE 12
#define SOCKET_PORT 51117
#define SOCKET_ADDR "192.168.1.219"
//------------------------------------------------------------------------------------------------
Display *display;
struct timespec tr;
struct timespec tw = {0,T_DELAY};
//------------------------------------------------------------------------------------------------
void main_color(XImage *image, int *colors);
