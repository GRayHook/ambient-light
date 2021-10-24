/* This transport sends color to Logitech-G19-Linux-Daemon.
 * See: https://github.com/GRayHook/Logitech-G19-Linux-Daemon/
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <color.h>

#define SOCKET_PORT 51117
#define SOCKET_ADDR "127.0.0.1"

void logitech_lcd_prepare();
void logitech_lcd_handler(colors_t * colors);
void logitech_lcd_exit();

color_handler_t logitech_lcd = {
	.prepare = logitech_lcd_prepare,
	.handler = logitech_lcd_handler,
	.exit    = logitech_lcd_exit
};

static int sock;

void logitech_lcd_prepare()
{
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror("logitech_lcd: socket");
		return;
	}

	struct sockaddr_in addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_port = htons(SOCKET_PORT);
	addr.sin_addr.s_addr = inet_addr(SOCKET_ADDR);

	if (connect(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
	{
		perror("logitech_lcd: connect");
		close(sock);
		sock = 0;
		return;
	}
}

void logitech_lcd_handler(colors_t * colors)
{
	if (!sock)
		return;

	if (send(sock, colors, sizeof(uint8_t) * 3, 0) < 1)
	{
		perror("logitech_lcd: send");
		close(sock);
		sock = 0;
		return;
	}
}

void logitech_lcd_exit()
{
	if (!sock)
		return;

	close(sock);
	sock = 0;
}

