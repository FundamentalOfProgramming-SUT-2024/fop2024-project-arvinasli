#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "library.h"

typedef struct {
    int x;
    int y;
} Pos;

typedef struct {
    char username[50];
    char password[50];
    char email[50];
    int score;
    int gold;
    int count_games;
    int experience;
} Player;

typedef struct {
    Pos room_pos;
    int room_size_v;
    int room_size_h;
    Pos doors[2];
    Pos pillar_seed;

} Room;

typedef struct {
    Player player;
    int difficulty;
    int player_color;
    Pos player_pos;
    Room rooms[6];
    int MAX_health;
    int players_health;
} Game;

void login(Player *p);
void create_account(Player *p);
void menu_handler(Player *p, Game *g);
int game_menu(Player *p);
void score_board(Player *p);
void settings(Player *p, Game *g);
void profile(Player *p);
void game_launcher(Player *p, Game *g);
void map_builder(Game *g, int initial_room);
int handle_movement(int *visited, int ch, Game *g, int *min_i, int *min_j);
void display_health(Game *g);
void corridor_path(char direction, Pos door1, Pos door2);
void display_screen(int *visited, Room *rooms, char **screen, int min_i, int min_j, int max_i, int max_j);
void floor_generator(int floor_number, Game *g);
int check_room(Room *rooms, int i, int j);

int main() {
    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    start_color();

    srand(time(NULL));

    Player p;
    login(&p);

    Game g;
    g.difficulty = 0; g.player_color = 0;
    menu_handler(&p,&g);
    
    endwin();
    return 0;
}

void login(Player *p) {
    clear();
    echo();
    curs_set(TRUE);

    draw_rogue();

    char username[50]; char password[50]; char score[50]; char gold[50]; char count_games[50];
    mvprintw(LINES / 2 - 2, COLS / 2 - 15, "Enter your username: ");
    getnstr(username, 50);
    mvprintw(LINES / 2 - 1, COLS / 2 - 15, "Enter your password: ");
    getnstr(password, 50);
    curs_set(FALSE);
    noecho();

    FILE *fptr;
    fptr = fopen("accounts.txt", "r");
    char line[50];
    int user_check = 0; int pass_check = 0;
    while (fgets(line, 50, fptr) != NULL) {
        line[strcspn(line, "\n")] = '\0';
        if (strcmp(username, line) == 0) {
            user_check = 1;
            strcpy(p->username,username);
            if (fgets(line, 50, fptr) != NULL) {
                line[strcspn(line, "\n")] = '\0';
                if (strcmp(password, line) == 0) {
                    pass_check = 1;
                    strcpy(p->password,password);
                    fgets(p->email, 50, fptr);
                    fgets(score, 50, fptr); score[strcspn(score, "\n")] = '\0';
                    fgets(gold, 50, fptr); gold[strcspn(gold, "\n")] = '\0';
                    fgets(count_games, 50, fptr); count_games[strcspn(count_games, "\n")] = '\0';
                    p->score = str_to_num(score); p->gold = str_to_num(gold); p->count_games = str_to_num(count_games);
                }
            }
            break;
        }
    }
    fclose(fptr);

    if(user_check==1 && pass_check==0) {
        mvprintw(LINES / 2 + 1, COLS / 2 - 15, "Your password is INCORRECT! Retry by running the game again.");
        mvprintw(LINES / 2 + 2, COLS / 2 - 15, "Press any key to EXIT");
        int ch = getch();
        endwin();
        exit(0);
    }
    else if(user_check==0 && pass_check==0) {
        mvprintw(LINES / 2 + 1, COLS / 2 - 15, "Your username and password are INCORRECT!");

        const char *options[] = {"Don't have an account? MAKE ONE", "EXIT"};
        int choice = 0;
        while (1) {
            for (int i=0; i<2; i++) {
                if (i == choice)
                    attron(A_REVERSE);
                mvprintw(LINES / 2 + 2 +i, COLS / 2 - 15, "%s", options[i]);
                if (i == choice)
                    attroff(A_REVERSE);
            }

            int ch = getch();
            if (ch == KEY_UP && choice == 1)
                choice = 0;
            else if (ch == KEY_DOWN && choice == 0)
                choice = 1;
            else if (ch == 10)
                break;
        }

        switch (choice) {
        case 0:
            create_account(p);
            login(p);
            break;
        case 1:
            endwin();
            exit(0);
            break;
        }
    }
    clear();
}

void create_account(Player *p) {
    clear();
    echo();
    curs_set(TRUE);

    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);

    attron(COLOR_PAIR(1));
    mvprintw(LINES / 2 - 6, COLS / 2 - 15, "CREATE YOUR ACCOUNT");
    attroff(COLOR_PAIR(1));

    FILE *fptr;

    do {
        mvprintw(LINES / 2 - 2, COLS / 2 - 15, "Enter your email: ");
        clrtobot();
        getnstr(p->email, 50);
        if(!check_email(p->email)) {
            attron(COLOR_PAIR(2));
            mvprintw(LINES / 2 - 4, COLS / 2 - 15, "Your email is invalid!      ");
            attroff(COLOR_PAIR(2));
        }
        else {
            attron(COLOR_PAIR(3));
            mvprintw(LINES / 2 - 4, COLS / 2 - 15, "Your email is valid.        ");
            attroff(COLOR_PAIR(3));
        }
    } while (!check_email(p->email));

    int check_username;
    do {
        fptr = fopen("accounts.txt", "r");
        check_username = 1;
        mvprintw(LINES / 2 - 1, COLS / 2 - 15, "Choose your username: ");
        clrtobot();
        getnstr(p->username, 50);
        char line[50];
        while (fgets(line, 50, fptr) != NULL) {
            line[strcspn(line, "\n")] = '\0';
            if (strcmp(p->username, line) == 0) {
                check_username = 0;
                attron(COLOR_PAIR(2));
                mvprintw(LINES / 2 - 4, COLS / 2 - 15, "This username exists!       ");
                attroff(COLOR_PAIR(2));
                break;
            }
        }
        if(check_username) {
            attron(COLOR_PAIR(3));
            mvprintw(LINES / 2 - 4, COLS / 2 - 15, "This username is valid.     ");
            attroff(COLOR_PAIR(3));
        }
        fclose(fptr); 
    } while (!check_username);

    do {
        mvprintw(LINES / 2, COLS / 2 - 15, "Choose your password: ");
        clrtobot();
        getnstr(p->password, 50);
        if(!check_password(p->password)) {
            attron(COLOR_PAIR(2));
            mvprintw(LINES / 2 - 4, COLS / 2 - 15, "Make sure to use uppercase, lowercase and digit values (at least 7 characters).");
            attroff(COLOR_PAIR(2));
        }
        else {
            fptr = fopen("accounts.txt", "a");
            fprintf(fptr, "%s\n", p->username); fprintf(fptr, "%s\n", p->password); fprintf(fptr, "%s\n", p->email);
            p->score = 0;
            p->gold = 0;
            p->count_games = 0;
            fprintf(fptr, "%d\n", p->score); fprintf(fptr, "%d\n", p->gold); fprintf(fptr, "%d\n", p->count_games);
            fclose(fptr);
        }
    } while (!check_password(p->password));
    
    curs_set(FALSE);
    noecho();
}

void menu_handler(Player *p, Game *g) {
    switch (game_menu(p)) {
        case 1:
            game_launcher(p,g);
            break;
        case 2:
            /* code */
            break;
        case 3:
            score_board(p);
            menu_handler(p,g);
            break;
        case 4:
            settings(p,g);
            menu_handler(p,g);
            break;
        case 5:
            profile(p);
            menu_handler(p,g);
            break;
        default:
            endwin();
            exit(0);
            break;
    }
}

int game_menu(Player *p) {
    clear();
    init_pair(1, COLOR_BLACK, COLOR_CYAN);

    attron(COLOR_PAIR(1));
    mvprintw(1, 1, "GAME MENU");
    attroff(COLOR_PAIR(1));
    mvprintw(2, 1, "Welcome %s", p->username);

    const char *options[] = {"Start a new Game", "Play your last Game", "Scoreboard", "Settings", "Profile"};
    int choice = 0;
    while (1) {
        for (int i=0; i<5; i++) {
            if (i == choice)
                attron(A_REVERSE);
            mvprintw(4+i, 1, "%s", options[i]);
            if (i == choice)
                attroff(A_REVERSE);
        }

        int ch = getch();
        if (ch == KEY_UP && choice != 0)
            choice--;
        else if (ch == KEY_DOWN && choice != 4)
            choice++;
        else if (ch == 10)
            break;
    }

    return choice+1;

    clear();
}

void score_board(Player *p) {
    clear();
    init_pair(1, COLOR_BLACK, COLOR_CYAN);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_MAGENTA, COLOR_BLACK);

    attron(COLOR_PAIR(1));
    mvprintw(1, 1, "SCORE BOARD");
    attroff(COLOR_PAIR(1));
    mvprintw(1, 20, "Press any key to RETURN");
    mvprintw(3, 1, "RANK"); mvprintw(3, 10, "username"); mvprintw(3, 24, "score"); mvprintw(3, 35, "gold"); mvprintw(3, 44, "gamecounts"); mvprintw(3, 58, "exp");

    char usernames[10][50]; int scores[10]; int golds[10]; int game_counts[10]; int exps[10] = {0};
    int arr[] = {0,1,2,3,4,5,6,7,8,9};
    int *ptr_arr = arr;

    FILE *fptr;
    fptr = fopen("accounts.txt","r");
    char line[50]; int i = 0; int count = 0;
    while(fgets(line, 50, fptr) != NULL) {
        line[strcspn(line,"\n")] = '\0';
        if(i%6 == 0) {
            if(i != 0) {
                count++;
            }
            strcpy(usernames[count],line);
        }
        else if(i%6 == 3) {
            scores[count] = str_to_num(line);
        }
        else if(i%6 == 4) {
            golds[count] = str_to_num(line);
        }
        else if(i%6 == 5) {
            game_counts[count] = str_to_num(line);
        }
        i++;
    }
    fclose(fptr);

    sort(ptr_arr, scores, (count%10)+1);
    attron(COLOR_PAIR(2));
    mvprintw(4, 76, "  .__.");
    mvprintw(5, 76, " (|  |)");
    mvprintw(6, 76, "  (  )");
    mvprintw(7, 76, "  _)(_");
    attroff(COLOR_PAIR(2));
    for(int i=0; i<=count; i++) {
        if(0<=i && i<=2) {
            attron(COLOR_PAIR(3));
            mvprintw(5+i, 66, "LEGEND");
            attroff(COLOR_PAIR(3));
            attron(COLOR_PAIR(2));
        }
        else{
            attroff(COLOR_PAIR(2));
        }
        if(strcmp(p->username,usernames[arr[i]]) == 0) {
            mvprintw(5+i, 1, "-> %d", i+1); mvprintw(5+i, 10, "%s", usernames[arr[i]]); mvprintw(5+i, 24, "%d", scores[arr[i]]); mvprintw(5+i, 35, "%d", golds[arr[i]]); mvprintw(5+i, 44, "%d", game_counts[arr[i]]); mvprintw(5+i, 58, "%d", exps[arr[i]]);
        }
        else {
            mvprintw(5+i, 1, "%d", i+1); mvprintw(5+i, 10, "%s", usernames[arr[i]]); mvprintw(5+i, 24, "%d", scores[arr[i]]); mvprintw(5+i, 35, "%d", golds[arr[i]]); mvprintw(5+i, 44, "%d", game_counts[arr[i]]); mvprintw(5+i, 58, "%d", exps[arr[i]]);
        }
        
    }

    char ch = getch();

    clear();
}

void settings(Player *p, Game *g) {
    clear();
    init_pair(1, COLOR_BLACK, COLOR_CYAN);

    attron(COLOR_PAIR(1));
    mvprintw(1, 1, "SETTINGS");
    attroff(COLOR_PAIR(1));
    mvprintw(1, 20, "Press ENTER after your changes");
    
    mvprintw(3, 1, "Difficulty");
    const char *difficulty[] = {"Easy", "Hard"};
    g->difficulty = 0;
    while (1) {
        for (int i=0; i<2; i++) {
            if (i == g->difficulty)
                attron(A_REVERSE);
            mvprintw(4+i, 1, "%s", difficulty[i]);
            if (i == g->difficulty)
                attroff(A_REVERSE);
        }

        int ch = getch();
        if (ch == KEY_UP && g->difficulty != 0)
            g->difficulty--;
        else if (ch == KEY_DOWN && g->difficulty != 1)
            g->difficulty++;
        else if (ch == 10)
            break;
    }

    mvprintw(7, 1, "Player's Color:");
    g->player_color = 0;
    const char *color[] = {"White", "Blue", "Red", "Green"};
    while (1) {
        for (int i=0; i<4; i++) {
            if (i == g->player_color)
                attron(A_REVERSE);
            mvprintw(8+i, 1, "%s", color[i]);
            if (i == g->player_color)
                attroff(A_REVERSE);
        }

        int ch = getch();
        if (ch == KEY_UP && g->player_color != 0)
            g->player_color--;
        else if (ch == KEY_DOWN && g->player_color != 3)
            g->player_color++;
        else if (ch == 10)
            break;
    }

    clear();
}

void profile(Player *p) {
    clear();
    init_pair(1, COLOR_BLACK, COLOR_YELLOW);

    attron(COLOR_PAIR(1));
    mvprintw(1, 1, "PROFILE");
    attroff(COLOR_PAIR(1));
    mvprintw(1, 20, "Press any key to RETURN");
    mvprintw(3, 1, "username: %s", p->username);
    mvprintw(4, 1, "email: %s", p->email);
    mvprintw(5, 1, "score: %d", p->score);
    mvprintw(6, 1, "gold: %d", p->gold);
    mvprintw(7, 1, "game counts: %d", p->count_games);

    char ch = getch();

    clear();
}

void game_launcher(Player *p, Game *g) {
    clear();
    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);

    if(g->difficulty) {
        g->MAX_health = 5;
    }
    else {
        g->MAX_health = 10;
    }
    g->players_health = g->MAX_health;

    p->count_games++;

    floor_generator(1, g);
    

    clear();
}

void floor_generator(int floor_number, Game *g) {
    clear();

    if(floor_number == 1) {
        display_message("You're now in the first floor!");
        map_builder(g,6);
    }
    else if(floor_number == 2) {
        display_message("You're now in the second floor!");
        map_builder(g,check_room(g->rooms,g->player_pos.x,g->player_pos.y));
    }
    else if(floor_number == 3) {
        display_message("You're now in the third floor!");
        map_builder(g,check_room(g->rooms,g->player_pos.x,g->player_pos.y));
    }
    else if(floor_number == 4) {
        display_message("You're now in the last floor!!!");
        map_builder(g,check_room(g->rooms,g->player_pos.x,g->player_pos.y));
    }



    for(int j=1; j<LINES; j++) {
        for(int i=0; i<COLS; i++) {
            for(int k=0; k<6; k++) {
                if(abs(j-g->rooms[k].room_pos.y) == g->rooms[k].room_size_v && abs(i-g->rooms[k].room_pos.x) <= g->rooms[k].room_size_h) {
                    mvprintw(j, i, "-");
                }
                else if(abs(i-g->rooms[k].room_pos.x) == g->rooms[k].room_size_h && abs(j-g->rooms[k].room_pos.y) < g->rooms[k].room_size_v) {
                    mvprintw(j, i, "|");
                }
                else if(abs(i-g->rooms[k].room_pos.x) < g->rooms[k].room_size_h && abs(j-g->rooms[k].room_pos.y) < g->rooms[k].room_size_v) {
                    mvprintw(j, i, ".");
                }
            }
        }
    }

    for(int k=0; k<6; k++) {
        g->rooms[k].pillar_seed.x = g->rooms[k].room_pos.x + power(-1,rand()%2)*(rand()%(g->rooms[k].room_size_h-3));
        g->rooms[k].pillar_seed.y = g->rooms[k].room_pos.y + power(-1,rand()%2)*(rand()%(g->rooms[k].room_size_v-3));
    }
    for(int k=0; k<6; k++) {
        int d1=1; int d2=1; int stairs = 0;
        for(int j=1; j<LINES; j++) {
            for(int i=0; i<COLS; i++) {
                if(k == 0) {
                    if(j-g->rooms[k].room_pos.y == g->rooms[k].room_size_v && abs(i-g->rooms[k].room_pos.x) <= g->rooms[k].room_size_h && d1) {
                        int new_i = i+rand()%g->rooms[k].room_size_h;
                        mvprintw(j, new_i, "+"); d1 = 0;
                        g->rooms[k].doors[1].x = new_i; g->rooms[k].doors[1].y = j;
                    }
                    else if(i-g->rooms[k].room_pos.x == g->rooms[k].room_size_h && abs(j-g->rooms[k].room_pos.y) < g->rooms[k].room_size_v && d2) {
                        int new_j = j+rand()%g->rooms[k].room_size_v;
                        mvprintw(new_j, i, "+"); d2 = 0;
                        g->rooms[k].doors[0].x = i; g->rooms[k].doors[0].y = new_j;
                    }
                }
                else if(k == 1) {
                    if(g->rooms[k].room_pos.y-j == g->rooms[k].room_size_v && abs(i-g->rooms[k].room_pos.x) <= g->rooms[k].room_size_h && d1) {
                        int new_i = i+rand()%g->rooms[k].room_size_h;
                        mvprintw(j, new_i, "+"); d1 = 0;
                        g->rooms[k].doors[0].x = new_i; g->rooms[k].doors[0].y = j;
                    }
                    else if(i-g->rooms[k].room_pos.x == g->rooms[k].room_size_h && abs(j-g->rooms[k].room_pos.y) < g->rooms[k].room_size_v && d2) {
                        int new_j = j+rand()%g->rooms[k].room_size_v;
                        mvprintw(new_j, i, "+"); d2 = 0;
                        g->rooms[k].doors[1].x = i; g->rooms[k].doors[1].y = new_j;
                    }
                }
                else if(k == 2 || k == 3) {
                    if(g->rooms[k].room_pos.x-i == g->rooms[k].room_size_h && abs(j-g->rooms[k].room_pos.y) < g->rooms[k].room_size_v && d1) {
                        int new_j = j+1;
                        mvprintw(new_j, i, "+"); d1 = 0;
                        g->rooms[k].doors[0].x = i; g->rooms[k].doors[0].y = new_j;
                    }
                    else if(i-g->rooms[k].room_pos.x == g->rooms[k].room_size_h && abs(j-g->rooms[k].room_pos.y) < g->rooms[k].room_size_v && d2) {
                        int new_j = j+rand()%g->rooms[k].room_size_v;
                        mvprintw(new_j, i, "+"); d2 = 0;
                        g->rooms[k].doors[1].x = i; g->rooms[k].doors[1].y = new_j;
                    }
                }
                else if(k == 4) {
                    if(j-g->rooms[k].room_pos.y == g->rooms[k].room_size_v && abs(i-g->rooms[k].room_pos.x) <= g->rooms[k].room_size_h && d1) {
                        int new_i = i+rand()%g->rooms[k].room_size_h;
                        mvprintw(j, new_i, "+"); d1 = 0;
                        g->rooms[k].doors[1].x = new_i; g->rooms[k].doors[1].y = j;
                    }
                    else if(g->rooms[k].room_pos.x-i == g->rooms[k].room_size_h && abs(j-g->rooms[k].room_pos.y) < g->rooms[k].room_size_v && d2) {
                        int new_j = j+rand()%g->rooms[k].room_size_v;
                        mvprintw(new_j, i, "+"); d2 = 0;
                        g->rooms[k].doors[0].x = i; g->rooms[k].doors[0].y = new_j;
                    }
                }
                else if(k == 5) {
                    if(g->rooms[k].room_pos.y-j == g->rooms[k].room_size_v && abs(i-g->rooms[k].room_pos.x) <= g->rooms[k].room_size_h && d1) {
                        int new_i = i+rand()%g->rooms[k].room_size_h;
                        mvprintw(j, new_i, "+"); d1 = 0;
                        g->rooms[k].doors[0].x = new_i; g->rooms[k].doors[0].y = j;
                    }
                    else if(g->rooms[k].room_pos.x-i == g->rooms[k].room_size_h && abs(j-g->rooms[k].room_pos.y) < g->rooms[k].room_size_v && d2) {
                        int new_j = j+rand()%g->rooms[k].room_size_v;
                        mvprintw(new_j, i, "+"); d2 = 0;
                        g->rooms[k].doors[1].x = i; g->rooms[k].doors[1].y = new_j;
                    }
                }

                if(i==g->rooms[k].pillar_seed.x && j==g->rooms[k].pillar_seed.y) {
                    if(rand()%2) {
                        mvprintw(j, i, "O"); mvprintw(j, i-1, "O"); mvprintw(j, i-2, "O"); mvprintw(j, i+1, "O"); mvprintw(j, i+2, "O");
                    }
                    else {
                        mvprintw(j, i, "O"); mvprintw(j-1, i, "O"); mvprintw(j+1, i, "O"); mvprintw(j+2, i, "O");
                    }
                }
                
                
                if(abs(i-g->rooms[k].room_pos.x) < g->rooms[k].room_size_h && abs(j-g->rooms[k].room_pos.y) < g->rooms[k].room_size_v) {
                    if(floor_number != 4) {
                        if(k == 3 || k == 4) {
                            if(rand()%2 == 0 && rand()%((g->rooms[k].room_size_h*g->rooms[k].room_size_v)) == 0 && stairs<2) {
                                mvprintw(j, i, "<");
                                stairs++;
                            }
                        }
                    }
                }
            }
        }
    }

    corridor_path('v',g->rooms[0].doors[1],g->rooms[1].doors[0]);
    corridor_path('h',g->rooms[0].doors[0],g->rooms[2].doors[0]);
    corridor_path('h',g->rooms[2].doors[1],g->rooms[4].doors[0]);
    corridor_path('h',g->rooms[1].doors[1],g->rooms[3].doors[0]);
    corridor_path('h',g->rooms[3].doors[1],g->rooms[5].doors[1]);
    corridor_path('v',g->rooms[4].doors[1],g->rooms[5].doors[0]);

    char **screen = (char **) malloc(COLS*sizeof(char*));
    for(int i=0; i<COLS; i++) {
        *(screen+i) = (char *) malloc(LINES*sizeof(char));
    }
    for(int j=1; j<LINES; j++) {
        for(int i=0; i<COLS; i++) {
            chtype character = mvinch(j,i);
            screen[i][j] = character;
        }
    }
    clear();

    int *visited = calloc(6,sizeof(int)); visited[check_room(g->rooms,g->player_pos.x,g->player_pos.y)] = 1;
    int display_whole = 0; int min_i = g->rooms[check_room(g->rooms,g->player_pos.x,g->player_pos.y)].room_pos.x - g->rooms[check_room(g->rooms,g->player_pos.x,g->player_pos.y)].room_size_h - 2; int min_j = g->rooms[check_room(g->rooms,g->player_pos.x,g->player_pos.y)].room_pos.y - g->rooms[check_room(g->rooms,g->player_pos.x,g->player_pos.y)].room_size_v - 2;
    display_screen(visited,g->rooms,screen,min_i,min_j,COLS,LINES);
    while(1) {
        if(display_whole%2 == 1) {
            for(int j=1; j<LINES; j++) {
                for(int i=0; i<COLS; i++) {
                    mvprintw(j,i,"%c",screen[i][j]);
                }
            }
        }
        else {
            display_screen(visited,g->rooms,screen,min_i,min_j,COLS,LINES);
        }
        attron(COLOR_PAIR(g->player_color));
        mvprintw(g->player_pos.y, g->player_pos.x, "@");
        attroff(COLOR_PAIR(g->player_color));
        display_health(g);
        int ch = getch();
        if(ch == 'm') {
            display_whole++;
            continue;
        }
        switch(handle_movement(visited, ch, g, &min_i, &min_j)) {
            case 1:
                floor_generator(floor_number+1, g);
                break;
        }
    }

}

void map_builder(Game *g, int initial_room) {
    int index = 0;
    for(int i=0; i<3; i++) {
        for(int j=0; j<2; j++) {
            if(index!=initial_room) {
                g->rooms[index].room_size_v = 4 + rand()%3;
                g->rooms[index].room_size_h = 8 + rand()%4;
                while(g->rooms[index].room_pos.x+g->rooms[index].room_size_h>=COLS || g->rooms[index].room_pos.x-g->rooms[index].room_size_h<=0 ||
                g->rooms[index].room_pos.y+g->rooms[index].room_size_v>=LINES || g->rooms[index].room_pos.y-g->rooms[index].room_size_v<=0) {
                    g->rooms[index].room_pos.x = rand() % ((COLS/6) + 1) + i*(COLS/3);
                    g->rooms[index].room_pos.y = rand() % ((LINES/4) + 1) + 1.5*j*(LINES/2);
                }
            }

            index++;
        }
    }
    if(initial_room == 6) {
        g->player_pos.x = g->rooms[5].room_pos.x;
        g->player_pos.y = g->rooms[5].room_pos.y;
    }

}

int handle_movement(int *visited, int ch, Game *g, int *min_i, int *min_j) {
    int count_dots = 0;
    chtype character = mvinch(g->player_pos.y-1, g->player_pos.x); if(character == '.') {count_dots++;}
    character = mvinch(g->player_pos.y+1, g->player_pos.x); if(character == '.') {count_dots++;}
    character = mvinch(g->player_pos.y, g->player_pos.x-1); if(character == '.') {count_dots++;}
    character = mvinch(g->player_pos.y, g->player_pos.x+1); if(character == '.') {count_dots++;}
    if(count_dots >= 2) {
        mvprintw(g->player_pos.y, g->player_pos.x, ".");
    }
    else if(count_dots == 1) {
        mvprintw(g->player_pos.y, g->player_pos.x, "+");
    }
    else {
        mvprintw(g->player_pos.y, g->player_pos.x, "#");
        if(abs(g->player_pos.x-g->rooms[0].room_pos.x) <= g->rooms[0].room_size_h+2 && abs(g->player_pos.y-g->rooms[0].room_pos.y) <= g->rooms[0].room_size_v+2) {visited[0] = 1; *min_i=min(*min_i,g->rooms[0].room_pos.x-g->rooms[0].room_size_h); *min_j=min(*min_j,g->rooms[0].room_pos.y-g->rooms[0].room_size_v);}
        else if(abs(g->player_pos.x-g->rooms[1].room_pos.x) <= g->rooms[1].room_size_h+2 && abs(g->player_pos.y-g->rooms[1].room_pos.y) <= g->rooms[1].room_size_v+2) {visited[1] = 1; *min_i=min(*min_i,g->rooms[1].room_pos.x-g->rooms[1].room_size_h); *min_j=min(*min_j,g->rooms[1].room_pos.y-g->rooms[1].room_size_v);}
        else if(abs(g->player_pos.x-g->rooms[2].room_pos.x) <= g->rooms[2].room_size_h+2 && abs(g->player_pos.y-g->rooms[2].room_pos.y) <= g->rooms[2].room_size_v+2) {visited[2] = 1; *min_i=min(*min_i,g->rooms[2].room_pos.x-g->rooms[2].room_size_h); *min_j=min(*min_j,g->rooms[2].room_pos.y-g->rooms[2].room_size_v);}
        else if(abs(g->player_pos.x-g->rooms[3].room_pos.x) <= g->rooms[3].room_size_h+2 && abs(g->player_pos.y-g->rooms[3].room_pos.y) <= g->rooms[3].room_size_v+2) {visited[3] = 1; *min_i=min(*min_i,g->rooms[3].room_pos.x-g->rooms[3].room_size_h); *min_j=min(*min_j,g->rooms[3].room_pos.y-g->rooms[3].room_size_v);}
        else if(abs(g->player_pos.x-g->rooms[4].room_pos.x) <= g->rooms[4].room_size_h+2 && abs(g->player_pos.y-g->rooms[4].room_pos.y) <= g->rooms[4].room_size_v+2) {visited[4] = 1; *min_i=min(*min_i,g->rooms[4].room_pos.x-g->rooms[4].room_size_h); *min_j=min(*min_j,g->rooms[4].room_pos.y-g->rooms[4].room_size_v);}
        if(ch == '8' && mvinch(g->player_pos.y-1, g->player_pos.x) != ' ') {
            *min_j -= 1;
        }
        else if(ch == '4' && mvinch(g->player_pos.y, g->player_pos.x-1) != ' ') {
            *min_i -= 1;
        }
    }



    switch (ch) {
        case '8':
            character = mvinch(g->player_pos.y-1, g->player_pos.x);
            if(character == '<') { return 1; }
            if (character == '.' || character == '+' || character == '#') { g->player_pos.y--; }
            break;
        case '2':
            character = mvinch(g->player_pos.y+1, g->player_pos.x);
            if(character == '<') { return 1; }
            if (character == '.' || character == '+' || character == '#') { g->player_pos.y++; }
            break;
        case '4':
            character = mvinch(g->player_pos.y, g->player_pos.x-1);
            if(character == '<') { return 1; }
            if (character == '.' || character == '+' || character == '#') { g->player_pos.x--; }
            break;
        case '6':
            character = mvinch(g->player_pos.y, g->player_pos.x+1);
            if(character == '<') { return 1; }
            if (character == '.' || character == '+' || character == '#') { g->player_pos.x++; }
            break;
        case '9':
            character = mvinch(g->player_pos.y-1, g->player_pos.x+1);
            if(character == '<') { return 1; }
            if (character == '.' || character == '+' || character == '#') { g->player_pos.y--; g->player_pos.x++; }
            break;
        case '7':
            character = mvinch(g->player_pos.y-1, g->player_pos.x-1);
            if(character == '<') { return 1; }
            if (character == '.' || character == '+' || character == '#') { g->player_pos.y--; g->player_pos.x--; }
            break;
        case '3':
            character = mvinch(g->player_pos.y+1, g->player_pos.x+1);
            if(character == '<') { return 1; }
            if (character == '.' || character == '+' || character == '#') { g->player_pos.y++; g->player_pos.x++; }
            break;
        case '1':
            character = mvinch(g->player_pos.y+1, g->player_pos.x-1);
            if(character == '<') { return 1; }
            if (character == '.' || character == '+' || character == '#') { g->player_pos.y++; g->player_pos.x--; }
            break;
        case 'f':
            ch = getch();
            if(ch == '8') {
                character = mvinch(g->player_pos.y-1, g->player_pos.x);
                while(character == '.' || character == '+' || character == '#') {
                    g->player_pos.y--;
                    character = mvinch(g->player_pos.y-1, g->player_pos.x);
                }
            }
            else if(ch == '2') {
                character = mvinch(g->player_pos.y+1, g->player_pos.x);
                while(character == '.' || character == '+' || character == '#') {
                    g->player_pos.y++;
                    character = mvinch(g->player_pos.y+1, g->player_pos.x);
                }
            }
            else if(ch == '4') {
                character = mvinch(g->player_pos.y, g->player_pos.x-1);
                while(character == '.' || character == '+' || character == '#') {
                    g->player_pos.x--;
                    character = mvinch(g->player_pos.y, g->player_pos.x-1);
                }
            }
            else if(ch == '6') {
                character = mvinch(g->player_pos.y, g->player_pos.x+1);
                while(character == '.' || character == '+' || character == '#') {
                    g->player_pos.x++;
                    character = mvinch(g->player_pos.y, g->player_pos.x+1);
                }
            }
            break;
    }
    return 0;
}

void display_health(Game *g) {
    mvprintw(0, COLS-2*g->MAX_health-10, "Health: ");
    mvprintw(0, COLS-2*g->MAX_health-2, "[");
    int count_hearts = g->players_health;
    attron(COLOR_PAIR(2));
    while(count_hearts) {
        mvprintw(0, COLS-2*count_hearts-1, "<3");
        count_hearts--;
    }
    attroff(COLOR_PAIR(2));
    mvprintw(0, COLS-1, "]");

}

void corridor_path(char direction, Pos door1, Pos door2) {
    if(direction == 'v') {
        int num = 1 + rand()%(door2.y-door1.y-1);
        int j=1;
        for(j; j<=num; j++) {
            mvprintw(door1.y+j,door1.x,"#");
        }
        j--;
        if(door1.x>door2.x) {
            for(int i=1; i<door1.x-door2.x; i++) {
                mvprintw(door1.y+j,door1.x-i,"#");
            }
        }
        else {
            for(int i=1; i<door2.x-door1.x; i++) {
                mvprintw(door1.y+j,door1.x+i,"#");
            }
        }
        for(j; j<door2.y-door1.y; j++) {
            mvprintw(door1.y+j,door2.x,"#");
        }
    }
    else if(direction == 'h') {
        int num = 1 + rand()%(door2.x-door1.x-1);
        int i=1;
        for(i; i<=num; i++) {
            mvprintw(door1.y,door1.x+i,"#");
        }
        i--;
        if(door1.y>door2.y) {
            for(int j=1; j<door1.y-door2.y; j++) {
                mvprintw(door1.y-j,door1.x+i,"#");
            }
        }
        else {
            for(int j=1; j<door2.y-door1.y; j++) {
                mvprintw(door1.y+j,door1.x+i,"#");
            }
        }
        for(i; i<door2.x-door1.x; i++) {
            mvprintw(door2.y,door1.x+i,"#");
        }
    }
  
}

void display_screen(int *visited, Room *rooms, char **screen, int min_i, int min_j, int max_i, int max_j) {
    for(int j=1; j<max_j; j++) {
        for(int i=0; i<max_i; i++) {
            int k = 0;
            for(k; k<6; k++) {
                if(abs(i-rooms[k].room_pos.x) <= rooms[k].room_size_h && abs(j-rooms[k].room_pos.y) <= rooms[k].room_size_v) {
                    break;
                }
            }
            if(i<min_i || j<min_j || (k!=6 && visited[k] == 0)) {
                mvprintw(j,i," ");
            }
            else {
                mvprintw(j,i,"%c",screen[i][j]);
            }
            
        }
    }
}

int check_room(Room *rooms, int i, int j) {
    for(int k=0; k<6; k++) {
        if(abs(i-rooms[k].room_pos.x) <= rooms[k].room_size_h && abs(j-rooms[k].room_pos.y) <= rooms[k].room_size_v) {
            return k;
        }
    }
}