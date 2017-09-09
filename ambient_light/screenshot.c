#include "screenshot.h"

int main() {

  Display *display;
  XImage *image;

  // Connect with Xserver
  if ((display = XOpenDisplay(getenv("DISPLAY"))) == NULL)
  {
    perror("Connect");
    exit(1);
  }

  image = XGetImage(display,
                    DefaultRootWindow(display), 0, 0,
                    DISPLAY_WIDTH,
                    DISPLAY_HEIGHT,
                    AllPlanes,
                    ZPixmap);

  XColor color;
  color.pixel = XGetPixel(image, 500, 500);
  XQueryColor (display,
               DefaultColormap(display, DefaultScreen (display)),
               &color);


  // Close connection with Xserver
  XCloseDisplay(display);

  printf("%d - %d - %d\n", color.red/C_DENOM, color.green/C_DENOM, color.blue/C_DENOM);

  return 0;
}
