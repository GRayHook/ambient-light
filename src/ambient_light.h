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
#define DISPLAY_WIDTH 2560
#define DISPLAY_HEIGHT 1080
#define PIXELS_STEP 50
#define T_DELAY 100000000
#define GREY_SENSETIVE 12
#define SOCKET_PORT 51117
#define SOCKET_ADDR "127.0.0.1"
#define PATHG13 "/tmp/g13-0"
//------------------------------------------------------------------------------------------------
Display *display;
struct timespec tr;
struct timespec tw = {0,T_DELAY};
typedef enum {
	RED = 0,
	GREEN = 1,
	BLUE = 2
} color_e;
//------------------------------------------------------------------------------------------------

FILE *right_fopen(char *path, char mode);
int check_file(char *filepath);
