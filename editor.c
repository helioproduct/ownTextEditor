/*** includes ***/

#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>

/*** defines ***/ 
#define CTRL_KEY(k) ((k) & 0x1f)


/*** data ***/

struct termios orig_termios; // Original terminal attr;


/*** terminal ***/  

void die(const char *s)
{   
    perror(s);
    exit(1);
}

void disableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    {
        die("tcsetattr");
    }
}

/*
    Canonical mode: Keyboard input is only sent
    when user press enter.
    This function disables canonical mode and 
    switches to the raw mode.
*/
void enableRawMode()
{
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    {
        die("tcsetattr");
    } 

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

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    {
        die("tcsetattr");
    }
}

char editorReadKey()
{
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) die("read");
    }
    return c;
}

void editorProcessKeyPress()
{
    char c = editorReadKey();

    switch (c) {
        case CTRL_KEY('q'):
            exit(0);
            break;
    }
}


/*** init ***/

int main()
{
    enableRawMode();

    while (1) {
        editorProcessKeyPress();
    }

    return 0;
}