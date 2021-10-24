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

void main_color(XImage * image, colors_t * colors)
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
		colors->red = 0xff;
		colors->green = 0xff;
		colors->blue = 0xff;
		return;
	}

	colors->red = (ared / pixels);
	colors->green = (agreen / pixels);
	colors->blue = (ablue / pixels);

	uint8_t * kmax = colors->green <= colors->red && colors->blue <= colors->red ? &colors->red :
	                 colors->red <= colors->green && colors->blue <= colors->green ? &colors->green :
	                 &colors->blue;
	uint8_t * kmin = colors->red <= colors->green && colors->red <= colors->blue ? &colors->red :
	                 colors->green <= colors->red && colors->green <= colors->blue ? &colors->green :
	                 &colors->blue;
	/* don't mind overflow */
	uint8_t * kmid = &colors->red + (unsigned long)&colors->green + (unsigned long)&colors->blue - (unsigned long)kmin - (unsigned long)kmax;

	/* Adjust saturation and lightness(brightness).
	 * Special case of HLS's Hue formula when Cmax == 100% and Cmin == 0%.
	 * We need to set greatest color to maximum and lessest to zero.
	 * Middle color calculates with respect to hue. */
	if (*kmid == *kmax)
		*kmid = 0xff;
	else if (*kmid == *kmin)
		*kmid = 0;
	else
		*kmid = (float)(*kmid - *kmin) / (float)(*kmax - *kmin) * 0xff;
	*kmax = 0xff;
	*kmin = 0;
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

		colors_t colors = { 0 };
		main_color(image, &colors);
		XDestroyImage(image);

		for (color_handler_t ** transport = transports; *transport != NULL; transport++)
			(*transport)->handler(&colors);

		nanosleep(&tw, &tr);
	}

	for (color_handler_t ** transport = transports; *transport != NULL; transport++)
		if ((*transport)->exit) (*transport)->exit();

	XCloseDisplay(display);

	return 0;
}
