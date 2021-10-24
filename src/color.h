#ifndef __COLORH
#define __COLORH
#include <stdint.h>

typedef struct {
	uint8_t red;
	uint8_t green;
	uint8_t blue;
} colors_t;

typedef struct {
	void (*prepare)();
	void (*handler)(colors_t * colors);
	void (*exit)();
} color_handler_t;

#endif
