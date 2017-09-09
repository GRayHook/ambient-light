#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
//------------------------------------------------------------------------------------------------
#define X 0
#define Y 0
#define WIDTH 600
#define HEIGHT 300
#define DISPLAY_WIDTH 1920
#define DISPLAY_HEIGHT 1080
//------------------------------------------------------------------------------------------------
extern int errno;
//------------------------------------------------------------------------------------------------
int main()
{
    Display     *display;
    Window      window;
    XEvent      event;
    int         screen;
    XImage      *image;
    GC          graph_ctx;
    XGCValues   *gc_values;

    // Соединяемся с X сервером
    if ((display = XOpenDisplay(getenv("DISPLAY"))) == NULL)
    {
        printf("Can't connect X server: %s\n", strerror(errno));
        exit(1);
    }

    screen = XDefaultScreen(display);
    image = XGetImage(display, DefaultRootWindow(display), 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT, AllPlanes, ZPixmap);
    // Создаем окно
    window = XCreateSimpleWindow(display, RootWindow(display, screen), X, Y, WIDTH, HEIGHT, 10, XBlackPixel(display, screen), XWhitePixel(display, screen));

    // Создаем графический контекст
    graph_ctx=XCreateGC(display, window, 0, gc_values);

    // На какие события будем реагировать
    XSelectInput(display, window, ExposureMask | KeyPressMask);

    //Показываем окно на экране
    XMapWindow(display, window);

    // Бесконечный цикл обработки событий
    while (1)
    {
        XNextEvent(display, &event);

        if (event.type == Expose) // Перерисовываем окно
        {
            // Отображаем картинку в окне
            XPutImage(display, window, graph_ctx, image, 0, 0, 0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT);
        }
        if (event.type == KeyPress) // При нажатии любой кнопки выходим
            break;
    }

    // Закрываем соединение с X сервером
    XCloseDisplay(display);

    return 0;
}
