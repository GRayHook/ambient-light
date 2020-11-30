/* This transport sends color to Logitech G13 driver by ecraven.
 * See: https://github.com/ecraven/g13
 */

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <color.h>

#define PATHG13 "/tmp/g13-0"

void g13_prepare();
void g13_handler(uint8_t * colors);
void g13_exit();

color_handler_t g13 = {
	.prepare = g13_prepare,
	.handler = g13_handler,
	.exit    = g13_exit
};

static FILE * fp;

int check_file(char * filepath)
{
	struct stat sb;
	if(stat(filepath, &sb) == -1) return 0;
	if(!S_ISFIFO(sb.st_mode)) return 0;
	if(sb.st_uid == getuid() && sb.st_mode & S_IWUSR) return 1;
	if(sb.st_gid == getgid() && sb.st_mode & S_IWGRP) return 1;
	if(sb.st_mode & S_IWOTH) return 1;
	return 0;
}

void g13_prepare()
{
	if(!check_file(PATHG13))
		return;

	if((fp = fopen(PATHG13, "w")) == NULL)
	{
		printf("G13: Unable open file (output).\n");
		return;
	}
}

void g13_handler(uint8_t * colors)
{
	if (!fp)
		return;

	fprintf(fp, "rgb %u %u %u", colors[RED], colors[GREEN], colors[BLUE]);
	fflush(fp);
}

void g13_exit()
{
	if (!fp)
		return;

	fclose(fp);
}
