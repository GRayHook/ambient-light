#include "ambient_light.h"

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

void send_g13(uint8_t * colors)
{
	int drisnya = check_file((char *)PATHG13);
	if(drisnya){
		FILE *fp;

		fp = right_fopen((char *)PATHG13, 'w');
		fprintf(fp, "rgb %u %u %u", colors[RED], colors[GREEN], colors[BLUE]);
		fclose(fp);
	}
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

int main() {
	XImage *image;
	uint8_t colors[3];
	int sock;
	struct sockaddr_in addr;

	if ((display = XOpenDisplay(getenv("DISPLAY"))) == NULL)
	{
		perror("Connect");
		exit(1);
	}

	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0) {
		perror("socket");
		exit(1);
	}
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SOCKET_PORT);
	addr.sin_addr.s_addr = inet_addr(SOCKET_ADDR);
	sleep(START_DELAY);
	if(connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
			perror("connect");
			exit(2);
	}

	while (1) {
		image = XGetImage(
			display,
			DefaultRootWindow(display), 0, 0,
			DISPLAY_WIDTH,
			DISPLAY_HEIGHT,
			AllPlanes,
			ZPixmap
		);
		main_color(image, colors);
		XDestroyImage(image);

		if (send(sock, colors, sizeof(uint8_t) * 3, 0) < 1) {
			perror("send");
			exit(0);
		}
		send_g13(colors);
		nanosleep(&tw, &tr);
	}

	XCloseDisplay(display);


	return 0;

}

FILE *right_fopen(char *path, char mode) {
	FILE *fp;
	if((fp=fopen(path, &mode))==NULL) {
		printf("Unable open file (output).\n");
		exit(1);
	}
	return fp;
}
int check_file(char *filepath){
		struct stat sb;
		if(stat(filepath, &sb) == -1) return 0;
		if(!S_ISFIFO(sb.st_mode)) return 0;
		if(sb.st_uid == getuid() && sb.st_mode & S_IWUSR) return 1;
		if(sb.st_gid == getgid() && sb.st_mode & S_IWGRP) return 1;
		if(sb.st_mode & S_IWOTH) return 1;
		return 0;
}
