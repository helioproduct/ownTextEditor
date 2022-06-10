#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>

struct termios orig_termios; // Original terminal attr;

void disableRawMode()
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}

// Canonical mode: Keyboard input is only sent
// when user press enter
// This function disables canonical mode and
// switches to the raw mode 
void enableRawMode()
{
    tcgetattr(STDIN_FILENO, &orig_termios);    
    atexit(disableRawMode);

    struct termios raw = orig_termios;

    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);

    // Ctrl-S and Ctrl-Q disable & Ctrl-M fix
    raw.c_lflag &= ~(ICRNL | IXON);

    // \n\r -> \n
    raw.c_oflag &= ~(OPOST);
    raw.c_oflag &= ~(CS8);

    // Disabling ECHO feture and special signals
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);

    // min number of bytes before read() returns
    raw.c_cc[VMIN] = 1;
   
    // max amount of time 
    raw.c_cc[VTIME] = 1;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main(void)
{
    enableRawMode();

    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q')
    {
        if (iscntrl(c)) {
            printf("%d\r\n", c);
        }  else {
            printf("%d ('%c')\r\n", c, c);
        }
    }

    return 0;
}