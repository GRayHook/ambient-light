#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <signal.h>

#include <color.h>
#include <handlers.h>

#define T_DELAY 25000000
#define PIXELS_STEP 11
#define GREY_SENSETIVE 12
#define LAST_COLORS_NUM 5

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
		/* skip three lines every line */
		if ((pixel - (uint32_t *)image->data) % image->width < PIXELS_STEP)
			pixel += image->width * 3;
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

	colors_t * colors = calloc(LAST_COLORS_NUM, sizeof(colors_t));
	unsigned last = 0;

	XImage *image;
	XShmSegmentInfo shminfo;

	image = XShmCreateImage(display, DefaultVisual(display,0), 24,
	                        ZPixmap, NULL, &shminfo, 100, 100);

	shminfo.shmid = shmget(IPC_PRIVATE,
	                       image->bytes_per_line * image->height,
	                       IPC_CREAT|0777);

	shminfo.shmaddr = image->data = shmat(shminfo.shmid, 0, 0);
	shminfo.readOnly = False;

	XShmAttach(display, &shminfo);

	while (!exit_flag)
	{
		XShmGetImage(display,
		             root, image, 0, 0,
		             AllPlanes);

		main_color(image, colors + last % LAST_COLORS_NUM);
		last++;

		int i = last % LAST_COLORS_NUM;
		unsigned long ared = 0,
		              agreen = 0,
		              ablue = 0;
		while((i = i < LAST_COLORS_NUM ? i + 1 : 0) != last % LAST_COLORS_NUM)
		{
			uint8_t red   = colors[i].red,
					green = colors[i].green,
					blue  = colors[i].blue;

			if (fabs(red - green) > GREY_SENSETIVE ||
				fabs(blue - green) > GREY_SENSETIVE)
			{
				ared += red;
				agreen += green;
				ablue += blue;
			}
		}

		colors_t color = {
			.red = (ared / LAST_COLORS_NUM),
			.green = (agreen / LAST_COLORS_NUM),
			.blue = (ablue / LAST_COLORS_NUM)
		};

		uint8_t * kmax = color.green <= color.red && color.blue <= color.red ? &color.red :
		                 color.red <= color.green && color.blue <= color.green ? &color.green :
		                 &color.blue;
		uint8_t * kmin = color.red <= color.green && color.red <= color.blue ? &color.red :
		                 color.green <= color.red && color.green <= color.blue ? &color.green :
		                 &color.blue;
		/* don't mind overflow */
		uint8_t * kmid = &color.red + (unsigned long)&color.green + (unsigned long)&color.blue - (unsigned long)kmin - (unsigned long)kmax;

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

		for (color_handler_t ** transport = transports; *transport != NULL; transport++)
			(*transport)->handler(&color);

		nanosleep(&tw, &tr);
	}

	XShmDetach(display, &shminfo);
	XDestroyImage(image);

	for (color_handler_t ** transport = transports; *transport != NULL; transport++)
		if ((*transport)->exit) (*transport)->exit();

	XCloseDisplay(display);

	return 0;
}
