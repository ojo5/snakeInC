#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>  // For usleep
#include <string.h> 

// Define keycodes for arrow keys and the exit key
#define LEFT_ARROW  XK_Left
#define RIGHT_ARROW XK_Right
#define UP_ARROW    XK_Up
#define DOWN_ARROW  XK_Down
#define EXIT_KEY    XK_q

// Function declarations
Window createWindow(Display *display, int screen, int x, int y, unsigned int width, unsigned int height);
GC createGraphicsContext(Display *display, Window window);
void handleExposeEvent(Display *display, Window window, GC gc, XEvent *event);
void updateBoxPosition(Display *display, Window window, GC gc, int newX, int newY);
void handleKeyPress(Display *display, Window window, GC gc, XEvent *event, int *boxX, int *boxY);
void cleanup(Display *display, Window window, GC gc);
void displayInitialScreen(Display *display, Window window, GC gc);
void displayInitialScreen(Display *display, Window window, GC gc);


void displayInitialScreen(Display *display, Window window, GC gc) {
    // Fill the window with a black background
    XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
    XFillRectangle(display, window, gc, 0, 0, 800, 800);

    // Load a font and set it to the GC
    XFontStruct *font = XLoadQueryFont(display, "fixed");
    if (font) {
        XSetFont(display, gc, font->fid);
    } else {
        fprintf(stderr, "Unable to load font 'fixed'.\n");
        exit(1);
    }

    // Set the color for the text (white)
    XSetForeground(display, gc, WhitePixel(display, DefaultScreen(display)));

    // Draw the text
    const char *text = "Press any key twice to start...";
    XDrawString(display, window, gc, 250, 400, text, strlen(text)); // Center the text vertically

    // Flush the display to ensure all drawing commands are processed
    XFlush(display);
}

Window createWindow(Display *display, int screen, int x, int y, unsigned int width, unsigned int height) {
    Window window;
    // Set the background to black instead of white
    window = XCreateSimpleWindow(display, RootWindow(display, screen), x, y, width, height, 1,
                                 BlackPixel(display, screen), BlackPixel(display, screen));  // Both the border and background are black
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

// Function to update box position and clear the screen
void updateBoxPosition(Display *display, Window window, GC gc, int newX, int newY) {
    // Clear the entire window by filling it with black
    XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
    XFillRectangle(display, window, gc, 0, 0, 800, 800);

    // Set the color for the new rectangle
    XSetForeground(display, gc, WhitePixel(display, DefaultScreen(display)));

    // Draw the box at the new position
    XFillRectangle(display, window, gc, newX, newY, 20, 20);
}

// Function to handle KeyPress events
void handleKeyPress(Display *display, Window window, GC gc, XEvent *event, int *boxX, int *boxY) {
    int moveAmount = 20; // Distance the box moves with each arrow key press
    KeySym key = XLookupKeysym(&event->xkey, 0);
    switch (key) {
        case LEFT_ARROW:
            *boxX -= moveAmount; // Move left
            break;
        case RIGHT_ARROW:
            *boxX += moveAmount; // Move right
            break;
        case UP_ARROW:
            *boxY -= moveAmount; // Move up
            break;
        case DOWN_ARROW:
            *boxY += moveAmount; // Move down
            break;
        case EXIT_KEY: // Press 'q' to exit
            cleanup(display, window, gc);
            exit(0);
            break;
        default:
            break;
    }

    // Ensure the box stays within the window boundaries
    if (*boxX < 0) *boxX = 0;
    if (*boxX > 780) *boxX = 780;
    if (*boxY < 0) *boxY = 0;
    if (*boxY > 780) *boxY = 780;

    // Update the box position
    updateBoxPosition(display, window, gc, *boxX, *boxY);
}

// Function to handle Expose events
void handleExposeEvent(Display *display, Window window, GC gc, XEvent *event) {
    if (event->type == Expose) {
        // Fill the window background with black
        XFillRectangle(display, window, DefaultGC(display, DefaultScreen(display)), 0, 0, 800, 800);

        // Draw 5 random boxes
        for (int i = 0; i < 5; i++) {
            int min = 0;
            int max = 780; // Adjusted to keep the box within the window
            int randomX = (rand() % (max - min + 1)) + min;
            int randomY = (rand() % (max - min + 1)) + min;

            // Draw the box at the new random position
            updateBoxPosition(display, window, gc, randomX, randomY);
        }
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
    int boxX = 400, boxY = 400; // Starting position of the box

    // Seed the random number generator
    srand(time(NULL));

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
            // Redraw the square if the window is exposed
            handleExposeEvent(display, window, gc, &event);
        } else if (event.type == KeyPress) {
            // Exit the loop after the first key press
            break;
        } else if (event.type == KeyPress) {
            handleKeyPress(display, window, gc, &event, &boxX, &boxY);
        }

        // Display the initial screen before the first key press
        displayInitialScreen(display, window, gc);
    }

    // Main event loop after the initial screen
    while (1) {
        XNextEvent(display, &event);
        
        if (event.type == Expose) {
            // Redraw the square if the window is exposed
            handleExposeEvent(display, window, gc, &event);
        } else if (event.type == KeyPress) {
            handleKeyPress(display, window, gc, &event, &boxX, &boxY);
        }
    }

    // Clean up and close the connection (though this will never be reached in the current loop)
    cleanup(display, window, gc);

    return 0;
}