#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>

// Function declarations
Window createWindow(Display *display, int screen, int x, int y, unsigned int width, unsigned int height);
GC createGraphicsContext(Display *display, Window window);
void handleExposeEvent(Display *display, Window window, GC gc, XEvent *event);
void cleanup(Display *display, Window window, GC gc);

// Function to create a simple X11 window
Window createWindow(Display *display, int screen, int x, int y, unsigned int width, unsigned int height) {
    Window window;
    window = XCreateSimpleWindow(display, RootWindow(display, screen), x, y, width, height, 1,
                                 BlackPixel(display, screen), WhitePixel(display, screen));
    XStoreName(display, window, "Snake Window");
    XSelectInput(display, window, ExposureMask | KeyPressMask);
    XMapWindow(display, window);
    return window;
}

// Function to create a graphics context
GC createGraphicsContext(Display *display, Window window) {
    GC gc = XCreateGC(display, window, 0, NULL);
    if (gc == NULL) {
        fprintf(stderr, "Failed to create GC\n");
        exit(1);
    }
    XSetForeground(display, gc, WhitePixel(display, DefaultScreen(display)));
    return gc;
}

// Function to handle Expose events
void handleExposeEvent(Display *display, Window window, GC gc, XEvent *event) {
    if (event->type == Expose) {
        // Fill the window background with black
        XFillRectangle(display, window, DefaultGC(display, DefaultScreen(display)), 0, 0, 800, 800);
        // Draw a square in the window
        XFillRectangle(display, window, gc, 400, 400, 20, 20);
    }
}

// Function to clean up resources
void cleanup(Display *display, Window window, GC gc) {
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

int main() {
    Display *display;
    Window window;
    GC gc;
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

    // Create the window
    window = createWindow(display, screen, 0, 0, 800, 800);

    // Create the graphics context
    gc = createGraphicsContext(display, window);

    // Main event loop
    while (1) {
        XNextEvent(display, &event);
        if (event.type == Expose) {
            handleExposeEvent(display, window, gc, &event);
        } else if (event.type == KeyPress) {
            break;
        }
    }

    // Clean up and close the connection
    cleanup(display, window, gc);

    return 0;
}