#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <signal.h>
#include <stdlib.h>

char board[20][35] = {0};
char clone[20][35] = {0};
int generation = 0;

void handle_sigint(int sig) {
    printf("\033[?1049l");
	exit(0);
}

int getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}

void render(int cx, int cy) {
    printf("\033[2J\033[H\n");
	printf("    \x1b[97mGeneration: %d\n    ........................................................................\n", generation);
    for (int y = 0; y < 20; y++) {
    	printf("    \x1b[97m.");
        for (int x = 0; x < 35; x++) {
			if (board[y][x] && cx == x && cy == y) printf("\x1b[38;5;245m██");
            else if (board[y][x]) printf("\x1b[97m██");
			else if (!board[y][x] && cx == x && cy == y) printf("\x1b[94m██");
            else printf("  ");
        }
        printf("\x1b[97m.\n");
    }
	printf("    \x1b[97m........................................................................\n");
    fflush(stdout);
}

void simulate() {
    memcpy(clone, board, sizeof(board));
    for (int y = 0; y < 20; y++) {
        for (int x = 0; x < 35; x++) {
            int n = 0;
            for (int ry = -1; ry <= 1; ry++) {
                for (int rx = -1; rx <= 1; rx++) {
                    if (rx == 0 && ry == 0) continue;
                    int ny = y + ry;
                    int nx = x + rx;
                    if (ny < 0 || ny >= 20 || nx < 0 || nx >= 35)
                        continue;
                    if (clone[ny][nx])
                        n++;
                }
            }
            if (clone[y][x]) {
                if (n < 2 || n > 3)
                    board[y][x] = 0;
                else
                    board[y][x] = 1;
            } else {
                if (n == 3)
                    board[y][x] = 1;
                else
                    board[y][x] = 0;
            }
        }
    }
	generation++;
}

int main() {
	signal(SIGINT, handle_sigint);
	printf("\033[?1049h");
	int cursor_x = 0, cursor_y = 0;
	while (1) {
    	render(cursor_x, cursor_y);
		char key = getch();
		switch (key) {
			case 'w': cursor_y--; if (cursor_y < 0) cursor_y = 0; break;
			case 's': cursor_y++; if (cursor_y > 19) cursor_y = 19; break;
			case 'a': cursor_x--; if (cursor_x < 0) cursor_x = 0; break;
			case 'd': cursor_x++; if (cursor_x > 34) cursor_x = 34; break;
			case 10: goto start; break;
			case 32: board[cursor_y][cursor_x] = !board[cursor_y][cursor_x];
		}
	}
	start:
    while (1) {
        simulate();
        render(-1, -1);
        usleep(150000);
    }
    return 0;
}
