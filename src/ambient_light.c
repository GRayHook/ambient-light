#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <signal.h>

#include <color.h>
#include <handlers.h>

#define T_DELAY 100000000
#define PIXELS_STEP 50
#define GREY_SENSETIVE 12

static int exit_flag = 0;

struct timespec tr;
struct timespec tw = {0,T_DELAY};

static inline uint8_t get_red(uint32_t pixel)
{
	return pixel >> 16 & 0xff;
}
static inline uint8_t get_green(uint32_t pixel)
{
	return pixel >> 8 & 0xff;
}
static inline uint8_t get_blue(uint32_t pixel)
{
	return pixel & 0xff;
}

void main_color(XImage * image, uint8_t * colors)
{
	unsigned int pixels = 0;
	unsigned long int ared = 0,
	                  agreen = 0,
	                  ablue = 0;

	uint32_t * pixel = (uint32_t *)image->data,
	         * last_pixel = ((uint32_t *)image->data) + image->width * image->height - 1;
	while(pixel <= last_pixel)
	{
		uint8_t red   = get_red(*pixel),
		        green = get_green(*pixel),
		        blue  = get_blue(*pixel);

		if (fabs(red - green) > GREY_SENSETIVE ||
			fabs(blue - green) > GREY_SENSETIVE)
		{
			ared += red;
			agreen += green;
			ablue += blue;
			pixels++;
		}

		pixel += PIXELS_STEP;
	}

	if (!pixels)
	{
		colors[RED] = 255;
		colors[GREEN] = 255;
		colors[BLUE] = 255;
		return;
	}

	colors[RED] = (ared / pixels);
	colors[GREEN] = (agreen / pixels);
	colors[BLUE] = (ablue / pixels);

	int kmax = colors[GREEN] <= colors[RED] && colors[BLUE] <= colors[RED] ? 0 :
	           colors[RED] <= colors[GREEN] && colors[BLUE] <= colors[GREEN] ? 1 :
	           2;
	int kmin = colors[RED] <= colors[GREEN] && colors[RED] <= colors[BLUE] ? 0 :
	           colors[GREEN] <= colors[RED] && colors[GREEN] <= colors[BLUE] ? 1 :
	           2;
	int kmid = (0 + 1 + 2) - kmin - kmax;

	/* Adjust saturation and lightness(brightness).
	 * Special case of HLS's Hue formula when Cmax == 100% and Cmin == 0%.
	 * We need to set greatest color to maximum and lessest to zero.
	 * Middle color calculates with respect to hue. */
	if (colors[kmid] == colors[kmax])
		colors[kmid] = 255;
	else if (colors[kmid] == colors[kmin])
		colors[kmid] = 0;
	else
		colors[kmid] = (float)(colors[kmid] - colors[kmin]) / (float)(colors[kmax] - colors[kmin]) * 255;
	colors[kmax] = 255;
	colors[kmin] = 0;
}

void term(int signum)
{
	exit_flag = 1;
}

int main()
{
	struct sigaction action = { 0 };
	action.sa_handler = term;
	sigaction(SIGTERM, &action, NULL);

	Display * display = XOpenDisplay(getenv("DISPLAY"));
	if (display == NULL)
	{
		perror("Connect");
		exit(1);
	}

	Window root = DefaultRootWindow(display);
	XWindowAttributes gwa = { 0 };
	XGetWindowAttributes(display, root, &gwa);
	int width = gwa.width,
	    height = gwa.height;

	for (color_handler_t ** transport = transports; *transport != NULL; transport++)
		if ((*transport)->prepare) (*transport)->prepare();

	while (!exit_flag)
	{
		XImage * image = XGetImage(display,
		                           root, 0, 0,
		                           width,
		                           height,
		                           AllPlanes,
		                           ZPixmap);

		uint8_t colors[3] = { 0 };
		main_color(image, colors);
		XDestroyImage(image);

		for (color_handler_t ** transport = transports; *transport != NULL; transport++)
			(*transport)->handler(colors);

		nanosleep(&tw, &tr);
	}

	for (color_handler_t ** transport = transports; *transport != NULL; transport++)
		if ((*transport)->exit) (*transport)->exit();

	XCloseDisplay(display);

	return 0;
}
