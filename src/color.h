#ifndef __COLORH
#define __COLORH
#include <stdint.h>

typedef enum {
	RED = 0,
	GREEN = 1,
	BLUE = 2
} color_e;

typedef struct {
	void (*prepare)();
	void (*handler)(uint8_t * colors);
	void (*exit)();
} color_handler_t;

#endif
