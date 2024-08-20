#include <X11/Xlib.h>
#include <stdio.h>
#include <stdlib.h>

Window createWindow(Display *display, int screen, int x, int y, unsigned int width, unsigned int height) {
    Window window;

    // Create the window
    window = XCreateSimpleWindow(display, RootWindow(display, screen),
                                 x, y, width, height, 1,
                                 BlackPixel(display, screen),
                                 WhitePixel(display, screen));

    // Set window properties
    XStoreName(display, window, "Snake Window");

    // Select input events
    XSelectInput(display, window, ExposureMask | KeyPressMask);

    // Map (show) the window
    XMapWindow(display, window);

    return window;
}

int main() {
    Display *display;
    Window window;
    XEvent event;
    int screen;

    // Open connection to X server
    display = XOpenDisplay(NULL);
    if (display == NULL) {
        fprintf(stderr, "Unable to open X display\n");
        exit(1);
    }

    // Get the default screen
    screen = DefaultScreen(display);

    // Create the window using the separate function
    window = createWindow(display, screen, 0, 0, 800, 800);

    // Main event loop
    while (1) {
        XNextEvent(display, &event);
        if (event.type == Expose) { 
            // Handle window expose event (redraw the window)
            XFillRectangle(display, window, DefaultGC(display, screen),
                           0, 0, 800, 800);
        } else if (event.type == KeyPress) {
            // Handle key press event (exit on any key)
            break;
        }
    }

    // Close the connection to the X server
    XCloseDisplay(display);
    return 0;
}