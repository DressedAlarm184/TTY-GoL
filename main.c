#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <signal.h>
#include <stdlib.h>

char board[30][45] = {0};
char clone[30][45] = {0};
int generation = 0, size = 0, speed = 150;
struct termios oldt, newt;

#define W (size == 0 ? 35 : 45)
#define H (size == 0 ? 20 : 30)

#define printff(...) do { \
    printf(__VA_ARGS__); \
    fflush(stdout); \
} while(0)

#define fgetchar() (tcflush(STDIN_FILENO, TCIFLUSH), getchar())

void cleanup() {
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	write(STDOUT_FILENO, "\033[?1049l\033[?25h", 14);
	_exit(0);
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

void checkdone() {
	if (memcmp(board, clone, sizeof board) == 0) {
		printff("    Press any key to exit...");
		fgetchar();
		cleanup();
	}
}

void initialize() {
	setvbuf(stdout, NULL, _IOFBF, 65536);
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt, newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	signal(SIGINT, cleanup);
	printff("\033[?1049h\033[2J\033[?25l");
}

void editor() {
	int cursor_x = 0, cursor_y = 0;
	while (1) {
		render(cursor_x, cursor_y);
		int key = getchar();
		if (key == EOF) cleanup();
		switch (key) {
			case 'w': cursor_y--; if (cursor_y < 0) cursor_y = 0; break;
			case 's': cursor_y++; if (cursor_y > H - 1) cursor_y = H - 1; break;
			case 'a': cursor_x--; if (cursor_x < 0) cursor_x = 0; break;
			case 'd': cursor_x++; if (cursor_x > W - 1) cursor_x = W - 1; break;
			case  10: case 13: return; break;
			case ' ': board[cursor_y][cursor_x] = !board[cursor_y][cursor_x]; break;
			case 'o': {
				printff("    Reset board and change size? [Y/N]");
				int ch = fgetchar();
				if (ch == 'Y' || ch == 'y') {
					size = !size;
					memset(board, 0, sizeof board);
					cursor_x = 0, cursor_y = 0;
				}
				printff("\033[2J");
				break;
			}
			case 'r': {
				printff("    Reset board? [Y/N]");
				int ch = fgetchar();
				if (ch == 'Y' || ch == 'y') memset(board, 0, sizeof board);
				printff("\033[2J");
				break;
			}
			case 'p': speed += 50; if (speed > 500) speed = 50; break;
		}
	}
}

int main() {
	initialize();
	editor();

	while (1) {
		simulate();
		render(-1, -1);
		usleep(speed * 1000);
		checkdone();
	}

	return 0;
}