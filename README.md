# Snake Game V.1
This repository contains a simple example of creating a window using the X Window System (Xlib) in C. The code demonstrates how to initialize a connection to the X server, create a window, handle basic events, and clean up resources.

## Overview
The provided code showcases how to create and display a basic X11 window on Linux. It sets up the window with a specific size, position, and colors. The code also includes a basic event loop to handle window exposure and key press events.

This project is a starting point for developing a more complex application, such as a Snake game, using the X Window System.

## Features
Create a simple window using Xlib.
Set window properties including title and size.
Handle basic X11 events like exposure and key presses.
Draw a filled rectangle within the window.

### Prerequisites
A Linux environment with Xlib development libraries installed.
GCC (GNU Compiler Collection) for compiling the code.
### Installation
Clone the Repository:

```sh
git clone https://github.com/your-username/x11-window-example.git
cd x11-window-example
```

#### Install Xlib Development Libraries:

On Debian-based systems (e.g., Ubuntu):

```sh
sudo apt-get install libx11-dev
```

#### Compile the Code:

```sh
gcc -o window window.c -lX11
```
#### Run the code:
```sh
./x11_window
Code Explanation
```

#### createWindow Function:

Purpose: Encapsulates the logic for creating a window.
Parameters:
Display *display: Connection to the X server.
int screen: Screen number (usually obtained with DefaultScreen).
int x, int y: Position of the window.
unsigned int width, unsigned int height: Dimensions of the window.
Returns: A Window identifier for the newly created window.
main Function:

Opens a connection to the X server.
Creates a window using the createWindow function.
Handles events in a loop (e.g., window exposure and key presses).
Closes the connection to the X server upon exit.

### To Do
Enhance the window with additional features such as interactive elements.
Develop a full Snake game with game mechanics and user controls.
Add graphical elements and animations.

### Contributing
Contributions are welcome! Please fork the repository and submit a pull request with your improvements or features. For major changes, please open an issue to discuss what you would like to change.

### License
This project is licensed under the BSD License - see the LICENSE file for details.

### Contact
If you have any questions or need further assistance, please feel free to contact me.
