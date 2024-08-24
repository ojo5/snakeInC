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

// Define directions
#define DIR_LEFT  0
#define DIR_RIGHT 1
#define DIR_UP    2
#define DIR_DOWN  3

// Define the maximum length of the trail
#define MAX_TRAIL_LENGTH 1000

// Function declarations
Window createWindow(Display *display, int screen, int x, int y, unsigned int width, unsigned int height);
GC createGraphicsContext(Display *display, Window window);
void handleExposeEvent(Display *display, Window window, GC gc, XEvent *event);
void updateBoxPosition(Display *display, Window window, GC gc, int boxX, int boxY, int redX, int redY, int score, int trail[][2], int trailLength);
void handleKeyPress(Display *display, Window window, GC gc, XEvent *event, int *direction);
void cleanup(Display *display, Window window, GC gc);
void displayInitialScreen(Display *display, Window window, GC gc);
int checkCollision(int boxX, int boxY, int redX, int redY);
int moveBox(int *boxX, int *boxY, int direction);

// Function to display the initial screen
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

// Function to create a window
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
void updateBoxPosition(Display *display, Window window, GC gc, int boxX, int boxY, int redX, int redY, int score, int trail[][2], int trailLength) {
    // Clear the entire window by filling it with black
    XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
    XFillRectangle(display, window, gc, 0, 0, 800, 800);

    // Draw the stationary red square
    XSetForeground(display, gc, 0xFF0000);  // Red color in hexadecimal (0xFF0000)
    XFillRectangle(display, window, gc, redX, redY, 20, 20);  // Stationary red square

    // Draw the trail
    XSetForeground(display, gc, WhitePixel(display, DefaultScreen(display)));
    for (int i = 0; i < trailLength; i++) {
        XFillRectangle(display, window, gc, trail[i][0], trail[i][1], 20, 20);
    }

    // Draw the moving white square (head of the trail)
    XFillRectangle(display, window, gc, boxX, boxY, 20, 20);

    // Display the score
    char scoreText[50];
    sprintf(scoreText, "Score: %d", score);
    XSetForeground(display, gc, WhitePixel(display, DefaultScreen(display)));
    XDrawString(display, window, gc, 10, 20, scoreText, strlen(scoreText));
}

// Function to check for collision between the white box and the red square
int checkCollision(int boxX, int boxY, int redX, int redY) {
    // Check if the white box overlaps with the red square
    return (boxX < redX + 20 && boxX + 20 > redX && boxY < redY + 20 && boxY + 20 > redY);
}

// Function to handle KeyPress events
void handleKeyPress(Display *display, Window window, GC gc, XEvent *event, int *direction) {
    KeySym key = XLookupKeysym(&event->xkey, 0);
    switch (key) {
        case LEFT_ARROW:
            *direction = DIR_LEFT;
            break;
        case RIGHT_ARROW:
            *direction = DIR_RIGHT;
            break;
        case UP_ARROW:
            *direction = DIR_UP;
            break;
        case DOWN_ARROW:
            *direction = DIR_DOWN;
            break;
        case EXIT_KEY: // Press 'q' to exit
            cleanup(display, window, gc);
            exit(0);
            break;
        default:
            break;
    }
}

// Function to move the box based on the current direction
int moveBox(int *boxX, int *boxY, int direction) {
    int moveAmount = 20; // Distance the box moves with each tick

    switch (direction) {
        case DIR_LEFT:
            *boxX -= moveAmount;
            break;
        case DIR_RIGHT:
            *boxX += moveAmount;
            break;
        case DIR_UP:
            *boxY -= moveAmount;
            break;
        case DIR_DOWN:
            *boxY += moveAmount;
            break;
    }

    // Check if the box hits the wall
    if (*boxX < 0 || *boxX >= 800 || *boxY < 0 || *boxY >= 800) {
        return 1; // Wall collision
    }

    return 0; // No wall collision
}

// Function to check if the white box hits itself
int checkSelfCollision(int boxX, int boxY, int trail[][2], int trailLength) {
    for (int i = 1; i < trailLength; i++) {
        if (boxX == trail[i][0] && boxY == trail[i][1]) {
            return 1; // Self-collision
        }
    }
    return 0; // No self-collision
}


// Function to handle Expose events
void handleExposeEvent(Display *display, Window window, GC gc, XEvent *event) {
    if (event->type == Expose) {
        // Redraw the box at the last known position
        // You can call updateBoxPosition with the last known position
    }
}

// Function to clean up resources
void cleanup(Display *display, Window window, GC gc) {
    XFreeGC(display, gc);
    XDestroyWindow(display, window);
    XCloseDisplay(display);
}

// Function to reset the game state
void resetGame(int *boxX, int *boxY, int *redX, int *redY, int *score, int trail[][2], int *trailLength, int *direction) {
    *boxX = 400;  // Reset to center of the window
    *boxY = 400;
    *redX = (rand() % 40) * 20;  // New random position for the red square
    *redY = (rand() % 40) * 20;
    *score = 0;  // Reset score
    *trailLength = 0;  // Clear the trail
    *direction = DIR_RIGHT;  // Reset direction

    // Optionally, you can also re-initialize the graphics context or do any other setup here
}

int main() {
    Display *display;
    Window window;
    GC gc;
    XEvent event;
    int screen;
    int boxX = 400, boxY = 400; // Starting position of the white box
    int redX = 200, redY = 200; // Starting position of the red square
    int score = 0;  // Initial score
    int trail[MAX_TRAIL_LENGTH][2];  // Array to store trail coordinates
    int trailLength = 0;  // Initial trail length
    int direction = DIR_RIGHT;  // Initial direction
    int tickCount = 0;  // Tick counter

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

    // Display the initial screen
    displayInitialScreen(display, window, gc);

    // Wait for the first key press to start the game
    while (1) {
        XNextEvent(display, &event);
        if (event.type == KeyPress) {
            handleKeyPress(display, window, gc, &event, &direction);
            break;
        }
    }

    
// Main game loop
while (1) {
    // Process pending events
    while (XPending(display)) {
        XNextEvent(display, &event);

        if (event.type == Expose) {
            handleExposeEvent(display, window, gc, &event);
        } else if (event.type == KeyPress) {
            handleKeyPress(display, window, gc, &event, &direction);
        }
    }

    // Increment tick counter
    tickCount++;

    // Move the box on every tick (adjust this number to change speed)
    if (tickCount % 5 == 0) {  // Adjust the modulus to control the speed
        // Move the box in the current direction
        int wallCollision = moveBox(&boxX, &boxY, direction);

        // Update the trail
        for (int i = trailLength; i > 0; i--) {
            trail[i][0] = trail[i - 1][0];
            trail[i][1] = trail[i - 1][1];
        }
        trail[0][0] = boxX;
        trail[0][1] = boxY;

        // Check for collision with the red square
        if (checkCollision(boxX, boxY, redX, redY)) {
            score++;
            if (trailLength < MAX_TRAIL_LENGTH) {
                trailLength++;
            }
            redX = (rand() % 40) * 20;
            redY = (rand() % 40) * 20;
        }

        // Check for self-collision or wall collision
        
        if (checkSelfCollision(boxX, boxY, trail, trailLength) || wallCollision) {
            resetGame(&boxX, &boxY, &redX, &redY, &score, trail, &trailLength, &direction);
        } else {
            // Redraw everything only if there’s no collision
            updateBoxPosition(display, window, gc, boxX, boxY, redX, redY, score, trail, trailLength);
        }

        // Sleep for a short duration to control the tick rate
        usleep(50000);  // Sleep for 50ms (20 ticks per second)
    }
}
}