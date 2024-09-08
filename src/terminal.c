#include <stdio.h>
#include <termios.h>
#include <unistd.h>

// Function to disable canonical mode and echo
void enable_raw_mode()
{
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty); // Get current terminal settings

    tty.c_lflag &= ~(ICANON | ECHO); // Disable canonical mode and echo

    tcsetattr(STDIN_FILENO, TCSANOW, &tty); // Apply settings
}

// Function to restore original terminal settings
void disable_raw_mode()
{
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty); // Get current terminal settings

    tty.c_lflag |= (ICANON | ECHO); // Enable canonical mode and echo

    tcsetattr(STDIN_FILENO, TCSANOW, &tty); // Apply settings
}
