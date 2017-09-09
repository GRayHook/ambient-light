#include "screenshot.h"

int main() {

  XImage *image;
  XColor color;
  if ((display = XOpenDisplay(getenv("DISPLAY"))) == NULL)
  {
    perror("Connect");
    exit(1);
  }


  while (1) {
    printf("Started!\n");
    image = XGetImage(
      display,
      DefaultRootWindow(display), 0, 0,
      DISPLAY_WIDTH,
      DISPLAY_HEIGHT,
      AllPlanes,
      ZPixmap
    );
    color = main_color(image);
    XDestroyImage(image);
    printf(
      "%d - %d - %d\n",
      color.red/C_DENOM,
      color.green/C_DENOM,
      color.blue/C_DENOM
    );
    nanosleep(&tw, &tr);
  }

  XCloseDisplay(display);


  return 0;

}

XColor main_color(XImage *image) {
  XColor tmp_clr, rslt_clr;
  unsigned int x = 0,
               y = 0,
               pixels = 0;
  unsigned long red = 0,
                green = 0,
                blue = 0;

  while(1) {
    tmp_clr.pixel = XGetPixel(image, x, y);
    XQueryColor (
      display,
      DefaultColormap(display, DefaultScreen (display)),
      &tmp_clr
    );

    if (
      fabs(tmp_clr.red - tmp_clr.green) > GREY_SENSETIVE &&
      fabs(tmp_clr.blue - tmp_clr.green) > GREY_SENSETIVE
    ) {
      red += tmp_clr.red/C_DENOM;
      green += tmp_clr.green/C_DENOM;
      blue += tmp_clr.blue/C_DENOM;
      pixels++;
    }

    x += rand() % PIXELS_STEP;
    if (x > DISPLAY_WIDTH - 1) {
      x = rand() % PIXELS_STEP;
      y += rand() % PIXELS_STEP;
    }
    if (y > DISPLAY_HEIGHT - 1) {
      break;
    }
  }

  rslt_clr.red = (red / pixels) * C_DENOM;
  rslt_clr.green = (green / pixels) * C_DENOM;
  rslt_clr.blue = (blue / pixels) * C_DENOM;

  return rslt_clr;
}
