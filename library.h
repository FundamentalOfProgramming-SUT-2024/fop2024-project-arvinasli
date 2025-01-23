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

void draw_cheese() {
    attron(COLOR_PAIR(4));
    mvprintw(2, 1, "       ___");
    mvprintw(3, 1, "     .'o O'-._");
    mvprintw(4, 1, "    / O o_.-`|");
    mvprintw(5, 1, "   /O_.-'  O |  LIST OF FOODS:");
    mvprintw(6, 1, "   | o   o .-`");
    mvprintw(7, 1, "   |o O_.-'");
    mvprintw(8, 1, "   '--`");
    attroff(COLOR_PAIR(4));
}

void draw_trophy(int x, int y) {
    mvprintw(y-4, x-4, "__");
    mvprintw(y-3, x-6, "(|  |)");
    mvprintw(y-2, x-5, "(  )");
    mvprintw(y-1, x-5, "_)(_");
}

void draw_sword() {
    attron(COLOR_PAIR(8));
    mvprintw(2, 5, "                           ___");
    mvprintw(3, 5, "                          ( ((");
    mvprintw(4, 5, "                           ) ))");
    mvprintw(5, 5, "  .::.                    / /(");
    mvprintw(6, 5, " 'M .-;-.-.-.-.-.-.-.-.-/| ((::::::::::::::::::::::::::::::::::::::::::::::.._");
    mvprintw(7, 5, "(J ( ( ( ( ( ( ( ( ( ( ( |  ))   -====================================-      _.>");
    mvprintw(8, 5, " `P `-;-`-`-`-`-`-`-`-`-\\| ((::::::::::::::::::::::::::::::::::::::::::::::''");
    mvprintw(9, 5, "  `::'                    \\ \\(");
    mvprintw(10, 5, "                           ) ))");
    mvprintw(11, 5, "                          (_((");
    attroff(COLOR_PAIR(8));
}

void draw_star() {
    attron(COLOR_PAIR(9));
    mvprintw(2, 2, "       .");
    mvprintw(3, 2, "  ---./|\\.---");
    mvprintw(4, 2, "  '._/ | \\_.'");
    mvprintw(5, 2, "_.-'_'.|.'_'-._");
    mvprintw(6, 2, " '-._.'|'._.-'");
    mvprintw(7, 2, "  .' \\ | / '.");
    mvprintw(8, 2, "  ---'\\|/'---");
    mvprintw(9, 2, "       '");
    attroff(COLOR_PAIR(9));
}

void draw_spiral(int num) {
    if(num%3 == 0) {
        attron(COLOR_PAIR(9));
        mvprintw(7, COLS/2+30, "       .");
        mvprintw(8, COLS/2+30, "      /|\\    ");
        mvprintw(9, COLS/2+30, "     / | \\   ");
        mvprintw(10, COLS/2+30, "     '.|.'     ");
        mvprintw(11, COLS/2+30, "     .'|'.    ");
        mvprintw(12, COLS/2+30, "     \\ | /   ");
        mvprintw(13, COLS/2+30, "      \\|/    ");
        mvprintw(14, COLS/2+30, "       '");

        mvprintw(7, COLS/2-42, "       .");
        mvprintw(8, COLS/2-42, "      /|\\    ");
        mvprintw(9, COLS/2-42, "     / | \\   ");
        mvprintw(10, COLS/2-42, "     '.|.'     ");
        mvprintw(11, COLS/2-42, "     .'|'.    ");
        mvprintw(12, COLS/2-42, "     \\ | /   ");
        mvprintw(13, COLS/2-42, "      \\|/    ");
        mvprintw(14, COLS/2-42, "       '");
        attroff(COLOR_PAIR(9));
    }
    
    else if(num%3 == 1) {
        attron(COLOR_PAIR(9));
        mvprintw(10, COLS/2+30, "_.-'_'. .'_'-._");
        mvprintw(11, COLS/2+30, " '-._.' '._.-'");

        mvprintw(10, COLS/2-42, "_.-'_'. .'_'-._");
        mvprintw(11, COLS/2-42, " '-._.' '._.-'");
        attroff(COLOR_PAIR(9));
    }

    else {
        attron(COLOR_PAIR(9));
        mvprintw(7, COLS/2+30, "       .");
        mvprintw(8, COLS/2+30, "  ---./|\\.---");
        mvprintw(9, COLS/2+30, "  '._/ | \\_.'");
        mvprintw(10, COLS/2+30, "_.-'_'.|.'_'-._");
        mvprintw(11, COLS/2+30, " '-._.'|'._.-'");
        mvprintw(12, COLS/2+30, "  .' \\ | / '.");
        mvprintw(13, COLS/2+30, "  ---'\\|/'---");
        mvprintw(14, COLS/2+30, "       '");

        mvprintw(7, COLS/2-42, "       .");
        mvprintw(8, COLS/2-42, "  ---./|\\.---");
        mvprintw(9, COLS/2-42, "  '._/ | \\_.'");
        mvprintw(10, COLS/2-42, "_.-'_'.|.'_'-._");
        mvprintw(11, COLS/2-42, " '-._.'|'._.-'");
        mvprintw(12, COLS/2-42, "  .' \\ | / '.");
        mvprintw(13, COLS/2-42, "  ---'\\|/'---");
        mvprintw(14, COLS/2-42, "       '");
        attroff(COLOR_PAIR(9));
    }
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

const char *txt_format(char username[]) {
    char *file_name = (char *) malloc(50*sizeof(char));
    strcpy(file_name,username);
    file_name[strlen(username)] = '.'; file_name[strlen(username)+1] = 't';
    file_name[strlen(username)+2] = 'x'; file_name[strlen(username)+3] = 't'; file_name[strlen(username)+4] = 0;
    return file_name;
}

int password() {
    int password = (1+rand()%9)*1000+ (rand()%10)*100 + (rand()%10)*10 + (rand()%10);
    return password;
}