#include "screenshot.h"

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

int main() {

	XImage *image;
	uint32_t colors[3];
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

		if (send(sock, colors, sizeof(uint32_t) * 3, 0) < 1) {
			perror("send");
			exit(0);
		}
		send_g13(colors);
		nanosleep(&tw, &tr);
	}

	XCloseDisplay(display);


	return 0;

}

void send_g13(int * colors) {
	int drisnya = check_file((char *)PATHG13);
	if(drisnya){
		FILE *fp;

		fp = right_fopen((char *)PATHG13, 'w');
		fprintf(fp, "rgb %d %d %d", colors[0], colors[1], colors[2]);
		fclose(fp);
	}
}
void main_color(XImage * image, uint32_t * colors) {
	XColor tmp_clr;
	unsigned int x = 0,
	             y = 0,
	             pixels = 0;
	unsigned long int ared = 0,
	                  agreen = 0,
	                  ablue = 0;

	uint32_t * pixel = (uint32_t *)image->data;
	while(1)
	{
		uint8_t red =   get_red(*pixel),
		        green = get_green(*pixel),
		        blue =  get_blue(*pixel);

		if (fabs(red - green) > GREY_SENSETIVE ||
			fabs(blue - green) > GREY_SENSETIVE)
		{
			ared += red;
			agreen += green;
			ablue += blue;
			pixels++;
		}

		pixel += PIXELS_STEP;
		if (pixel >= ((uint32_t *)image->data) + DISPLAY_HEIGHT * DISPLAY_WIDTH)
			break;
	}

	if (pixels) {
		float k = 0;
		int k1 = -1,
				k2 = -1,
				k3 = -1;

		colors[0] = (ared / pixels);
		colors[1] = (agreen / pixels);
		colors[2] = (ablue / pixels);
		for (int i = 0; i < 3; i++) {
			if (
				colors[i] >= colors[0] &&
				colors[i] >= colors[1] &&
				colors[i] >= colors[2] &&
				k1 == -1
			) {
				k1 = i;
			} else if (
				colors[i] <= colors[0] &&
				colors[i] <= colors[1] &&
				colors[i] <= colors[2] &&
				k3 == -1
			) {
				k3 = i;
			} else {
				k2 = i;
			}
		}
		k = (float)255 / (float)colors[k1];
		colors[k1] = 255;
		colors[k2] *= k;
		colors[k3] *= k;
	} else {
		colors[0] = 255;
		colors[1] = 255;
		colors[2] = 255;
	}

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
