#include <stdio.h>
#include <unistd.h>
#include <string.h>

char board[20][35] = {0};
char clone[20][35] = {0};

void render() {
    printf("\033[2J\033[H");
	printf("........................................................................\n");
    for (int y = 0; y < 20; y++) {
    	printf(".");
        for (int x = 0; x < 35; x++) {
            if (board[y][x]) printf("██");
            else printf("  ");
        }
        printf(".");
        printf("\n");
    }
	printf("........................................................................\n");
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
}

int main() {
    board[0][1] = 1;
    board[1][2] = 1;
    board[2][0] = 1;
    board[2][1] = 1;
    board[2][2] = 1;

    while (1) {
        simulate();
        render();
        usleep(300000);
    }

    return 0;
}
