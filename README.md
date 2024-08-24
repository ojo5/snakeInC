# Snake Game v1.0.0
This repository contains a simple implementation of the Snake game using the X Window System (Xlib) in C. The code demonstrates how to initialize a connection to the X server, create a window, handle basic events, and implement game mechanics.

## Overview
The provided code sets up a window with a specific size and position and handles basic X11 events such as exposure and key presses. The game features a white box that moves in response to arrow keys, a red square that acts as a target, and a trail that grows as the player collects points.

## Features
Create a window using Xlib.
Handle key presses for moving the box.
Draw a moving white box and a stationary red square.
Implement basic game mechanics including collision detection and scoring.
Update the game state based on user input.


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
```

### To Do
Fix known bugs
    2 key presses to start
    Able to change 180 direction

### Contributing
Contributions are welcome! Please fork the repository and submit a pull request with your improvements or features. For major changes, please open an issue to discuss what you would like to change.

### License
This project is licensed under the BSD License - see the LICENSE file for details.

### Contact
If you have any questions or need further assistance, please feel free to contact me.
