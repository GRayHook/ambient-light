#include "screenshot.h"

int main() {

  XImage *image;
  int colors[3];
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
  addr.sin_addr.s_addr = inet_addr("192.168.1.219");
  sleep(60);
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
    printf(
      "%d - %d - %d\n",
      colors[0],
      colors[1],
      colors[2]
    );
    if (send(sock, colors, sizeof(int) * 3, 0) < 1) {
      perror("send");
      exit(0);
    }
    nanosleep(&tw, &tr);
  }

  XCloseDisplay(display);


  return 0;

}

void main_color(XImage *image, int *colors) {
  XColor tmp_clr;
  unsigned int x = 0,
               y = 0,
               pixels = 0;
  unsigned int red = 0,
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
      fabs(tmp_clr.red/C_DENOM - tmp_clr.green/C_DENOM) > GREY_SENSETIVE &&
      fabs(tmp_clr.blue/C_DENOM - tmp_clr.green/C_DENOM) > GREY_SENSETIVE
    ) {
      red += tmp_clr.red/C_DENOM;
      green += tmp_clr.green/C_DENOM;
      blue += tmp_clr.blue/C_DENOM;
      pixels++;
    }

    x += 1 + rand() % PIXELS_STEP;
    if (x > DISPLAY_WIDTH - 1) {
      x = 1 + rand() % PIXELS_STEP;
      y += 1 + rand() % PIXELS_STEP;
    }
    if (y > DISPLAY_HEIGHT - 1) {
      break;
    }
  }
  printf("%d\n", pixels);
  if (pixels) {
    float k = 0;
    int k1 = -1,
        k2 = -1,
        k3 = -1;

    colors[0] = (red / pixels);
    colors[1] = (green / pixels);
    colors[2] = (blue / pixels);
    printf(
      "%d - %d - %d\n",
      colors[0],
      colors[1],
      colors[2]
    );
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
    k = (float)63 / (float)colors[k1];
    printf("%f %d %d %d\n", k, k1, k2, k3);
    colors[k1] *= k;
    colors[k2] *= k;
    colors[k3] *= k;
  } else {
    colors[0] = 63;
    colors[1] = 63;
    colors[2] = 63;
  }

}
