#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <ncurses.h>
#include <string.h>
#include <stdbool.h>

char* getFileBuffer(char* filename, long long* size);
char* printText(WINDOW* win, char* buf, bool* retIsEOF);

int main(int argc, char* argv[]) {
    WINDOW* win = initscr();
    if (win == NULL) {
        perror("Error: Couldn't create ncurses window");
        exit(1);
    }
    noecho();
    curs_set(0);

    if (argc < 2) {
        fputs("Error: No filename provided\n", stderr);
        exit(1);
    }

    long long file_size;
    char* file_buf = getFileBuffer(argv[1], &file_size);

    int win_x = -1, win_y = -1;
    getmaxyx(win, win_y, win_x);

    printw("File name: %s | File size: %lli bytes\n", argv[1], file_size);
    refresh();

    WINDOW* text_win = newwin(win_y - 1, win_x, 1, 0);
    if (win == NULL) {
        perror("Error: Couldn't create ncurses text window");
        free(file_buf);
        endwin();
        exit(1);
    }

    bool isEOF = false;
    char* text_off = file_buf;
    while (true) {
        werase(text_win);
        char* next_off = NULL;
        next_off = printText(text_win, text_off, &isEOF);
        wrefresh(text_win);

INPUT_KEYS:
        switch (getch()) {
            case 27:
                goto CLEAN_EXIT;
            case ' ': 
                if (!isEOF) {
                    text_off = next_off;
                    break;
                }
            default:
                goto INPUT_KEYS;
        }
    }

CLEAN_EXIT:
    free(file_buf);
    delwin(text_win);
    endwin();
    return 0;
}

char* getFileBuffer(char* filename, long long* size) {
    struct stat st;
    if (stat(filename, &st) == -1) {
        perror("Error: stat couldn't get file info");
        exit(1);
    }

    FILE *fd = fopen(filename, "rb");
    if (fd == NULL) {
        perror("Error: Couldn't open file");
        exit(1);
    }

    char *file_buf = malloc(st.st_size + 1);
    if (file_buf == NULL) {
        perror("Error: Failed to allocate memory");
        fclose(fd);
        exit(1);
    }

    if (fread(file_buf, 1, st.st_size, fd) != st.st_size) {
        perror("Error: couldn't read whole file");
        free(file_buf);
        fclose(fd);
        exit(1);
    }
    file_buf[st.st_size] = '\0';
    fclose(fd);

    if (size) *size = st.st_size;
    return file_buf;
}

char* printText(WINDOW* win, char* buf, bool* retIsEOF) {
    int win_h = -1, win_w = -1;
    getmaxyx(win, win_h, win_w);
    int text_h = win_h - 2, text_w = win_w - 2;

    box(win, 0, 0);

    bool isEOF = false;
    int str_len = 0;
    char* text_off = buf, *end_off = buf;
    for (int i = 0; i < text_h; i++) {
        end_off = strchr(text_off, '\n');
        if (!end_off) {
            end_off = strchr(text_off, '\0');
            isEOF = true;
        }
        str_len = end_off - text_off;

        wmove(win, 1 + i, 1);
        waddnstr(win, text_off, (str_len < text_w) ? str_len : text_w);

        if (isEOF) break;
        text_off = end_off + 1;
    }

    if (retIsEOF) *retIsEOF = isEOF;
    return text_off;
}
