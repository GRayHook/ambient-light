#include "transports/logitech_lcd.h"
#include "transports/g13.h"

color_handler_t * transports[] = {
	&logitech_lcd,
	&g13,
	NULL
};
