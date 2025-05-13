#include <stdio.h>
#include <unistd.h>
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <signal.h>

//gcc snake.c -lncurses

// Author: Julie
// Desc: Prints out a snake pit that borders the current terminal.
void printBorder() {
    for(int i = 0; i <= LINES; i++) {
        for(int k = 0; k <= COLS; k++) {
            if(i == 0 || i == LINES-1) {
                mvaddstr(i, k, "#"); // Outputs the top and bottom rows of the border
            } else if(k == 0 || k == COLS-1) {
                mvaddstr(i, k, "#"); // Outputs the edges of the border
            }
        } // Identifies the current column number
    } // Identifies the current row number
} 

// Global variables initializing data being passed between multiple functions
int trophyCheck = 0; 
int randRow;
int randCol;
int trophyValue;
char trophyChar;

// Desc: Removes the current trophy when the time limit is reached
void removeTrophy(int sigNum) {
    mvprintw(randRow, randCol, " ");
    trophyCheck = 0;
}

// Desc: Randomly generates a trophy
void generateTrophy() {
    int timeLimit;
    int maxRow = LINES - 1;
    int maxCol = COLS - 1;
    int min = 1;
    int max = 10;

    // When the time limit is reached, removes the current trophy
    signal(SIGALRM, removeTrophy);

    // (Psuedo) Random number generator to generate information about the trophy
    srand(time(NULL));
    // When the time limit has been reached, regenerate the trophy
    if(trophyCheck == 0) {
        // Ensures that the trophy won't spawn on the snake's tail
        while(1) {
            // Generates the position of the trophy
            randRow = min + (rand() % (maxRow - min));
            randCol = min + (rand() % (maxCol - min));
            if(mvinch(randRow, randCol) != '*') break;
        }
        // Generates the value of the trophy and the number of seconds until expiration
        trophyValue = min + (rand() % (max - min));
        timeLimit = min + (rand() % (max - min));
        // Prints the trophy in regards to the above information
        mvprintw(randRow, randCol, "%d", trophyValue);  
        trophyCheck = 1; 
        trophyChar = trophyValue + '0'; 
        // Sets the alarm to the randomized time limit 
        alarm(timeLimit);
    }
}
// Author: Julie [END]

// Author: Jake
// Desc: Handles snake movement and events (collisions)
void game_loop() {
    curs_set(0);
    noecho();
    keypad(stdscr, TRUE);

    // These two variables should be equal to the snake pit size.
    int pitSize = LINES*COLS;

    // Initializing our snake...
    int tailX[pitSize]; // Stores every head location for its eventual deletion.
    int tailY[pitSize]; 
    int tailLength = 3; // Initial snake length...
    int tailEndCounter = 0; // Keeps track of the tail end so it can be cleared.
    int headCounter = 0; // Keeps track of the head.
    int speed = 100000; // Initial speed.
    
    // We need to print our snake, so we go to a safe spot and do so (the middle).
    int x = COLS/2 - tailLength;
    int y = LINES/2;

    int lastInput; // This will store values in case of no user input.
    int ch;

    // Randomly selecting a start direction...
    srand(time(NULL));
    switch (rand() % 4) {
        case 0:
            ch = KEY_UP;
            break;
        case 1:
            ch = KEY_DOWN;
            break;
        case 2: 
            ch = KEY_LEFT;
            break;
        case 3:
            ch = KEY_RIGHT;
            break;
    }

    int run = 1;

    while (run) {
        lastInput = ch; 
        timeout(50);
        ch = getch(); 
        flushinp(); // The terminal will buffer inputs, this clears that buffer.
        if (ch == ERR) // If we don't get an input we'll just use our last one.
            ch = lastInput;

        switch (ch) {
            case KEY_UP: 
                y--;
                break;
            
            case KEY_DOWN: 
                y++;
                break;
            
            case KEY_LEFT: 
                x--;
                break;
            
            case KEY_RIGHT: 
                x++;
                break; 
            
            case 'x':       
            case 'X':
                endwin();
                return;
        }
        
        // mvinch gives the char at a position. This checks our new position for any special chars and can act accordingly.
        if (mvinch(y, x) == '*') {
            break;
        }   
          if (mvinch(y, x) == '#') {
            break;
        }   
        
        // The modulus allows for a circular array.
        tailX[(headCounter) % pitSize ] = x;
        tailY[(headCounter) % pitSize ] = y;
        headCounter++;
        
         if (headCounter - tailEndCounter > tailLength) {
            move(tailY[(tailEndCounter) % pitSize], tailX[(tailEndCounter) % pitSize]);
            addch(' ');
            tailEndCounter++;
        }

        // Author: Julie [START]
        if(mvinch(y, x) == trophyChar) {
            tailLength += trophyValue;
            mvprintw(randRow, randCol, "*");
            trophyCheck = 0;
            // Jake [START]
            if (tailLength > (COLS+LINES)) {
                run = 0;
                clear();
                mvprintw(LINES/2, COLS/2 - 5, "YOU WINNER");
                refresh();
                sleep(10);
            }
            // Jake [END]
        }
        // Author: Julie [END]

        // Printing our new head
        move(y,x);
        addch('*');

        generateTrophy();
        refresh();
        // Framerate basically. I like this nokia styled framerate.
        usleep(speed/(tailLength*.15));
    }
}


int main() {
    initscr();
    // We should only have to draw the border once. Make sure that it scales with the window!
    printBorder();
    // The actual game part will run in a game loop.
    game_loop();
    endwin();
    return 0;
}


