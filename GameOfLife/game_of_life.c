//run gcc main.c -lncurses
#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>

#define HEIGHT 25
#define WIDTH 80

int base_ceil_rule(int x, int y, int **field);
void evolve_ceil(int **field, int x, int y, int (*eval_rule)(int x, int y, int **field), int **buffer);
void evolve_field(int **field, int lenght, int height, int type, int (*eval_rule)(int x, int y, int **field),
                  int **buffer);
void load_buffer(int **field, int lenght, int height, int **buffer);
int alloc(int ***field, int lenght, int height);
int input_field(int **field, int lenght, int height);
void delloc(int **field);

void update_parametrs(int *speed, int *cursor_x, int *cursor_y, int **field);
void enable_raw_mode();
void disable_raw_mode();

void update_screen();
void print_field(int **field, int lenght, int height);
void print_menu();
void print_stats(int generation, int speed);
void print_border();

int main() {
    int **field = 0, **buffer = 0;
    int is_runing = 1;
    alloc(&field, WIDTH, HEIGHT);
    alloc(&buffer, WIDTH, HEIGHT);
    input_field(field, WIDTH, HEIGHT);
    if (freopen("/dev/tty", "r", stdin) == NULL) {
        printf("Exception, console dosent exists");
    }

    int speed = 10;
    int pause = 1;
    int edit = 0;
    int cursor_x = 39;
    int cursor_y = 12;
    int show_hud = 1;
    int show_menu = 1;
    int generation = 0;
    int frame = 0;

    initscr();
    raw();
    noecho();
    curs_set(0);
    nodelay(stdscr, 1);
    keypad(stdscr, TRUE);

    print_field(field, WIDTH, HEIGHT);
    print_border();

    while (is_runing) {
        if (!pause && (frame % (1 << speed)) == 0) {
            evolve_field(field, WIDTH, HEIGHT, 1, &base_ceil_rule, buffer);
            load_buffer(field, WIDTH, HEIGHT, buffer);
            generation++;
        }

        int input_char = getch();

        if (input_char == 'q' || input_char == 27) {
            is_runing = 0;
        } else if (input_char == '-') {
            if (speed < 10) {
                frame = 0;
                speed++;
            }
        } else if (input_char == '+') {
            if (speed > 0) {
                frame = 0;
                speed--;
            }
        } else if (input_char == ' ') {
            if (speed > 0) {
                frame = 0;
                pause = (pause == 0) ? 1 : 0;
                edit = 0;
                curs_set(0);
            }
        } else if (input_char == 'm') {
            show_menu = !show_menu;
        } else if (input_char == 'v') {
            show_hud = !show_hud;
        } else if (input_char == 'e' && pause) {
            edit = !edit;
            curs_set(edit);
        } else if (input_char == 'w' && edit) {
            cursor_y = (cursor_y - 1) % HEIGHT;
            cursor_y = cursor_y < 0 ? cursor_y + HEIGHT : cursor_y;
        } else if (input_char == 'a' && edit) {
            cursor_x = (cursor_x - 1) % WIDTH;
            cursor_x = cursor_x < 0 ? cursor_x + WIDTH : cursor_x;
        } else if (input_char == 's' && edit) {
            cursor_y = (cursor_y + 1) % HEIGHT;
        } else if (input_char == 'd' && edit) {
            cursor_x = (cursor_x + 1) % WIDTH;
        } else if (input_char == 10 && edit) {
            field[cursor_x][cursor_y] = 1;
        } else if ((input_char == KEY_BACKSPACE || input_char == 127) && edit) {
            field[cursor_x][cursor_y] = 0;
        }

        clear();
        print_field(field, WIDTH, HEIGHT);
        print_stats(generation, 10 - speed);
        if (show_hud == 1) {
            print_border();
        }
        if (show_menu == 1 && show_hud == 1) {
            print_menu();
        }
        if (edit) {
            move(cursor_y, cursor_x);
        }
        refresh();
        frame = (frame + 1) % 1024;
    }
    endwin();
    delloc(field);
    delloc(buffer);
    return 0;
}

int input_field(int **field, int lenght, int height) {
    int status = 0;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < lenght; x++) {
            if (status == 0) {
                status += !scanf("%d", &(field[x][y]));
            }
        }
    }
    return status;
}

int alloc(int ***field, int lenght, int height) {
    int status = 1;
    *field = (int **)malloc(sizeof(int *) * lenght + sizeof(int) * lenght * height);
    if (*field == NULL) {
        status = 0;
    }
    if (status) {
        int *ptr = (int *)((*field) + lenght);
        for (int i = 0; i < lenght; i++) {
            (*field)[i] = ptr + height * i;
        }
    }
    return status;
}

void delloc(int **field) {
    if (field != 0) {
        free(field);
    }
}

void evolve_field(int **field, int lenght, int height, int type, int (*eval_rule)(int x, int y, int **field),
                  int **buffer) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < lenght; x++) {
            if (field[x][y] == type || field[x][y] == 0) {
                evolve_ceil(field, x, y, eval_rule, buffer);
            }
        }
    }
}

void evolve_ceil(int **field, int x, int y, int (*eval_rule)(int x, int y, int **field), int **buffer) {
    buffer[x][y] = eval_rule(x, y, field);
}

int base_ceil_rule(int x, int y, int **field) {
    int count = 0;
    int new_state = 0;
    for (int i = -1; i <= 1; i++) {
        for (int j = -1; j <= 1; j++) {
            int shift_x = (x + i) % WIDTH, shift_y = (y + j) % HEIGHT;
            if (shift_x < 0) {
                shift_x += WIDTH;
            }
            if (shift_y < 0) {
                shift_y += HEIGHT;
            }
            if (!(i == 0 && j == 0) && field[shift_x][shift_y] == 1) {
                count++;
            }
        }
    }
    if (field[x][y] == 0 && count == 3) {
        new_state = 1;
    } else if (field[x][y] == 1 && (count == 2 || count == 3)) {
        new_state = 1;
    }
    return new_state;
}

void load_buffer(int **field, int lenght, int height, int **buffer) {
    for (int x = 0; x < lenght; x++) {
        for (int y = 0; y < height; y++) {
            field[x][y] = buffer[x][y];
        }
    }
}

void print_field(int **field, int lenght, int height) {
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < lenght; x++) {
            if (field[x][y] == 1) {
                mvaddch(y, x, '*');
            }
        }
    }
}

void print_border() {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (x == 0 && y == 0) {
                mvaddch(y, x, '.');
            } else if (x == WIDTH - 1 && y == 0) {
                mvaddch(y, x, '.');
            } else if (x == 0 && y == HEIGHT - 1) {
                mvaddch(y, x, '\'');
            } else if (x == WIDTH - 1 && y == HEIGHT - 1) {
                mvaddch(y, x, '\'');
            } else if (x == 0 || x == WIDTH - 1) {
                mvaddch(y, x, '|');
            } else if (y == 0 || y == HEIGHT - 1) {
                mvaddch(y, x, '-');
            }
        }
    }
}

void print_menu() {
    mvprintw(2, 1, "[+, -] - change speed      ");
    mvprintw(3, 1, "[SPACE] - pause game       ");
    mvprintw(4, 1, "[m] - hide menu            ");
    mvprintw(5, 1, "[v] - hide hud             ");
    mvprintw(6, 1, "[e] - enter edit mode      ");
    mvprintw(7, 1, "[w, a, s, d] - move cursor ");
}

void print_stats(int generation, int speed) {
    char stats[80];
    sprintf(stats, "Speed: %d | Generation: %d", speed, generation);
    mvprintw(1, 1, stats);
}