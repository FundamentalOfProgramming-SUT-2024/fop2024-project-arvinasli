#include <ncurses.h>
#include <stdlib.h>
#include <string.h>

void draw_rogue() {
    init_pair(1, COLOR_RED, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    mvprintw(LINES / 2 - 12, COLS / 2 - 23, " ____                                      ");
    mvprintw(LINES / 2 - 11, COLS / 2 - 23, "/\\  _`\\                                    ");
    mvprintw(LINES / 2 - 10, COLS / 2 - 23, "\\ \\ \\L\\ \\    ___      __   __  __     __   ");
    mvprintw(LINES / 2 - 9, COLS / 2 - 23, " \\ \\ ,  /   / __`\\  /'_ `\\/\\ \\/\\ \\  /'__`\\ ");
    mvprintw(LINES / 2 - 8, COLS / 2 - 23, "  \\ \\ \\\\ \\ /\\ \\L\\ \\/\\ \\L\\ \\ \\ \\_\\ \\/\\  __/ ");
    mvprintw(LINES / 2 - 7, COLS / 2 - 23, "   \\ \\_\\ \\_\\ \\____/\\ \\____ \\ \\____/\\ \\____\\");
    mvprintw(LINES / 2 - 6, COLS / 2 - 23, "    \\/_/\\/ /\\/___/  \\/___L\\ \\/___/  \\/____/");
    mvprintw(LINES / 2 - 5, COLS / 2 - 23, "                      /\\____/              ");
    mvprintw(LINES / 2 - 4, COLS / 2 - 23, "                      \\_/__/               ");
    attroff(COLOR_PAIR(1));
}

int str_to_num (char *str) {
    int num = 0;
    int p = strlen(str)-1;
    for(p; p>=0; p--) {
        num = num*10 + (str[strlen(str)-p-1]-'0');
    }
    
    return num;
}

int check_email(char email[50]) {
    int atsign_pos = 0; int dot_pos = 0;
    for(int i=0; i<strlen(email); i++) {
        if(email[i] == '@') {
            atsign_pos = i;
        }
        else if(email[i] =='.') {
            dot_pos = i;
        }
    }
    if(atsign_pos>=3 && dot_pos-atsign_pos>=2 && strlen(email)-dot_pos==4) {
        return 1;
    }
    return 0;
}

int check_password(char password[50]) {
    int upper = 0; int lower = 0; int digit = 0;
    for(int i=0; i<strlen(password); i++) {
        if (48<=password[i] && password[i]<=57) {
            digit = 1;
        }
        else if (65<=password[i] && password[i]<=90) {
            upper = 1;
        }
        else if (97<=password[i] && password[i]<=122) {
            lower = 1;
        }
    }

    if(strlen(password)>=7 && upper && lower && digit) {
        return 1;
    }
    return 0;
}

int power(int num, int n) {
    if(n==0) {
        return 1;
    }
    if(n==1) {
        return num;
    }
    for(int i=2; i<=n; i++) {
        num *= num;
    }
    return num;
}

int min(int a, int b) {
    if(a < b) {
        return a;
    }
    return b;
}

int max(int a, int b) {
    if(a > b) {
        return a;
    }
    return b;
}

void sort(int *arr, int scores[], int size) {
    for(int i=0; i<size-1; i++) {
        for(int j=i+1; j<size; j++) {
            if(scores[i] < scores[j]) {
                int temp = *(arr+i);
                *(arr+i) = *(arr+j);
                *(arr+j) = temp;
            }
        }
    }
}