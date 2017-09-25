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
#include <sys/types.h>
#include <sys/stat.h>
//------------------------------------------------------------------------------------------------
#define START_DELAY 1
#define DISPLAY_WIDTH 1920
#define DISPLAY_HEIGHT 1080
#define C_DENOM 1024
#define PIXELS_STEP 25
#define T_DELAY 400000000
#define GREY_SENSETIVE 12
#define SOCKET_PORT 51117
#define SOCKET_ADDR 192.168.1.219
#define PATHG13 "/tmp/g13-0"
//------------------------------------------------------------------------------------------------
Display *display;
struct timespec tr;
struct timespec tw = {0,T_DELAY};
//------------------------------------------------------------------------------------------------
void send_g13(int * colors);
void main_color(XImage *image, int *colors);

FILE *right_fopen(char *path, char mode);
int check_file(char *filepath);
