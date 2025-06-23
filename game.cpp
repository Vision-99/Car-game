#include <iostream>
#include <conio.h>
#include <windows.h>
#include <cstdlib>
#include <ctime>

using namespace std;

#define SCREEN_WIDTH  90
#define SCREEN_HEIGHT 26
#define WIN_WIDTH     70

HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
COORD CursorPosition;

// Enemy state
int enemyX[2];
int enemyY[2];
bool enemyActive[2];

// Player (“car”) shape: 4×4 block
char carShape[4][4] = {
    {' ', 'A', 'A', ' '},
    {'A', 'A', 'A', 'A'},
    {' ', 'A', 'A', ' '},
    {'A', 'A', 'A', 'A'}
};

int carPosX = WIN_WIDTH/2;
int score = 0;

// Move the console cursor
void gotoxy(int x, int y) {
    CursorPosition.X = x;
    CursorPosition.Y = y;
    SetConsoleCursorPosition(console, CursorPosition);
}

// Hide or show the blinking cursor
void setCursorVisible(bool visible) {
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = visible;
    SetConsoleCursorInfo(console, &info);
}

// Draw the left/right borders
void drawBorder() {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        gotoxy(0, y);             cout << '|';
        gotoxy(WIN_WIDTH, y);     cout << '|';
        gotoxy(SCREEN_WIDTH, y);  cout << '|';
    }
}

// Generate a new random X-position for an enemy
void spawnEnemy(int i) {
    enemyX[i] = 2 + rand() % (WIN_WIDTH - 4);
    enemyY[i] = 1;
    enemyActive[i] = true;
}

// Draw an enemy as a 4×4 block of '#'
void drawEnemy(int i) {
    if (!enemyActive[i]) return;
    for (int dy = 0; dy < 4; dy++) {
        gotoxy(enemyX[i], enemyY[i] + dy);
        if(dy%2==0) cout << "****";
        else cout<< " ** ";
    }
}

// Erase the 4×4 enemy block
void eraseEnemy(int i) {
    if (!enemyActive[i]) return;
    for (int dy = 0; dy < 4; dy++) {
        gotoxy(enemyX[i], enemyY[i] + dy);
        cout << "    ";
    }
}

// Draw the player’s car
void drawCar() {
    for (int dy = 0; dy < 4; dy++) {
        gotoxy(carPosX, SCREEN_HEIGHT - 5 + dy);
        for (int dx = 0; dx < 4; dx++) {
            cout << carShape[dy][dx];
        }
    }
}

// Erase the car
void eraseCar() {
    for (int dy = 0; dy < 4; dy++) {
        gotoxy(carPosX, SCREEN_HEIGHT - 5 + dy);
        cout << "    ";
    }
}

// Check collision between car and the first enemy
bool checkCollision() {
    // only check enemy[0] for simplicity
    if (!enemyActive[0]) return false;
    if (enemyY[0] + 4 >= SCREEN_HEIGHT - 5) {
        // horizontal overlap?
        if (abs(enemyX[0] - carPosX) < 4) {
            return true;
        }
    }
    return false;
}

// Display game-over screen
void gameOver() {
    system("cls");
    cout << "\n\n\n"
         << "\t===== GAME OVER =====\n"
         << "\t  Final Score: " << score << "\n\n"
         << "\tPress any key to return to menu...";
    getch();
}

// Update the score display
void updateScore() {
    gotoxy(WIN_WIDTH + 5, 2);
    cout << "Score: " << score;
}

// Show instructions screen
void instructions() {
    system("cls");
    cout << "Instructions:\n"
         << " - Avoid the incoming cars (####)\n"
         << " - Move left:  A\n"
         << " - Move right: D\n"
         << " - Quit:       ESC\n\n"
         << "Press any key to return to menu...";
    getch();
}

// The main gameplay loop
void play() {
    // reset state
    carPosX = WIN_WIDTH/2;
    score    = 0;

    // spawn the first enemy, second will appear later
    spawnEnemy(0);
    enemyActive[1] = false;

    system("cls");
    drawBorder();
    updateScore();

    while (true) {
        // input
        if (kbhit()) {
            char ch = getch();
            if ((ch == 'a' || ch == 'A') && carPosX > 1)
                carPosX -= 4;
            else if ((ch == 'd' || ch == 'D') && carPosX + 4 < WIN_WIDTH)
                carPosX += 4;
            else if (ch == 27) // ESC
                return;
        }

        // draw
        drawCar();
        drawEnemy(0);
        drawEnemy(1);

        // collision?
        if (checkCollision()) {
            gameOver();
            return;
        }

        Sleep(50);

        // erase for next frame
        eraseCar();
        eraseEnemy(0);
        eraseEnemy(1);

        // move enemies down
        if (enemyActive[0]) enemyY[0]++;
        if (enemyActive[1]) enemyY[1]++;

        // spawn second enemy when the first is halfway down
        if (enemyY[0] > SCREEN_HEIGHT/2 && !enemyActive[1]) {
            spawnEnemy(1);
        }

        // if an enemy goes off‐screen, reset it and bump score
        for (int i = 0; i < 2; i++) {
            if (enemyActive[i] && enemyY[i] > SCREEN_HEIGHT - 4) {
                eraseEnemy(i);
                spawnEnemy(i);
                score++;
                updateScore();
            }
        }
    }
}

int main() {
    srand((unsigned)time(nullptr));
    setCursorVisible(false);

    while (true) {
        system("cls");
        gotoxy(10,5);  cout << "=== Car Dodge Game ===";
        gotoxy(10,7);  cout << "1. Start Game";
        gotoxy(10,8);  cout << "2. Instructions";
        gotoxy(10,9);  cout << "3. Quit";
        gotoxy(10,11); cout << "Choose: ";

        char choice = getche();
        if (choice == '1')      play();
        else if (choice == '2') instructions();
        else if (choice == '3') break;
    }

    return 0;
}
