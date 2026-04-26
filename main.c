#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <signal.h>
#include <stdlib.h>

char board[30][45] = {0};
char clone[30][45] = {0};
int generation = 0, size = 0, speed = 150;

#define W (size == 0 ? 35 : 45)
#define H (size == 0 ? 20 : 30)

void handle_sigint(int sig) {
    write(1, "\033[?1049l", 8);
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
    printf("\033[H\n\x1b[97m    Generation: %d <> Size: %s <> Speed: %d ms       \n    ", generation, size ? "45x30" : "35x20", speed);
	if (size) printf("╔══════════════════════════════════════════════════════════════════════════════════════════╗\n");
	else printf("╔══════════════════════════════════════════════════════════════════════╗\n");
    for (int y = 0; y < H; y++) {
    	printf("    \x1b[97m║");
        for (int x = 0; x < W; x++) {
			if (board[y][x] && cx == x && cy == y) printf("\x1b[38;5;245m██");
            else if (board[y][x]) printf("\x1b[97m██");
			else if (!board[y][x] && cx == x && cy == y) printf("\x1b[94m██");
            else printf("  ");
        }
        printf("\x1b[97m║\n");
    }
	if (size)  printf("    \x1b[97m╚══════════════════════════════════════════════════════════════════════════════════════════╝\n");
	else printf("    \x1b[97m╚══════════════════════════════════════════════════════════════════════╝\n");
    fflush(stdout);
}

void simulate() {
    memcpy(clone, board, sizeof(board));
    for (int y = 0; y < H; y++) {
        for (int x = 0; x < W; x++) {
            int n = 0;
            for (int ry = -1; ry <= 1; ry++) {
                for (int rx = -1; rx <= 1; rx++) {
                    if (rx == 0 && ry == 0) continue;
                    int ny = y + ry;
                    int nx = x + rx;
                    if (ny < 0 || ny >= H || nx < 0 || nx >= W)
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
	printf("\033[?1049h\033[2J");
	int cursor_x = 0, cursor_y = 0;
	while (1) {
    	render(cursor_x, cursor_y);
		char key = getch();
		switch (key) {
			case 'w': cursor_y--; if (cursor_y < 0) cursor_y = 0; break;
			case 's': cursor_y++; if (cursor_y > H - 1) cursor_y = H - 1; break;
			case 'a': cursor_x--; if (cursor_x < 0) cursor_x = 0; break;
			case 'd': cursor_x++; if (cursor_x > W - 1) cursor_x = W - 1; break;
			case  10: goto start; break;
			case ' ': board[cursor_y][cursor_x] = !board[cursor_y][cursor_x]; break;
			case 'o': size = !size; printf("\033[2J"); break;
			case 'p': speed += 50; if (speed > 500) speed = 50; break;
		}
	}
	start:
    while (1) {
        simulate();
        render(-1, -1);
        usleep(speed * 1000);
    }
    return 0;
}
