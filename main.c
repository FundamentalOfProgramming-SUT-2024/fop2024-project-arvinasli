#include <ncursesw/ncurses.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
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
    int alive;
    int type;
    int room;
    Pos position;
    int radius;
    int on;
    int health;
    int damage;
    int haunt;
    int freeze;
} Monster;

typedef struct {
    int type;
    int locked;
    int password;
    Pos room_pos;
    int room_size_v;
    int room_size_h;
    Pos doors[2];
    Pos pillar_seed;
    int traps_count;
    Pos traps[10];
    int gold;
    int dark_gold;
    int ordinary_food;
    int potion;
    int monsters_count;
    Monster monsters[8];
} Room;

typedef struct {
    Player player;
    int difficulty;
    int player_color;
    Pos player_pos;
    Room rooms[6];
    int MAX_health;
    int hungriness_rate;
    int players_health;
    int players_extra_health;
    int players_score;
    int players_gold;
    int floor_number;
    int players_hungriness;
    int players_ordinary_food;
    int players_mace;
    int players_dagger;
    int players_magic_wand;
    int players_arrow;
    int players_sword;
    int players_health_potion;
    int players_speed_potion;
    int players_speed;
    int players_damage_potion;
    time_t start_time;
    int k_lock;
    time_t password_start_time;
    int secret_doors_count;
    Pos secret_doors[3];
    time_t enchant_start_time;
    int players_weapon;
    int players_weapon_direction;
    int players_steps;
    int players_message_step;
    int players_speed_step;
    int players_health_step;
    int players_damage_step;
    Pos weapons[3];
    int players_damage_rate;
    int players_ancient_key;
    int players_broken_key;
    int not_grab;
    int play_music;
} Game;

void login(Player *p);
void forgot_password(Player *p);
void create_account(Player *p);
void menu_handler(Player *p, Game *g);
int game_menu(Player *p);
void score_board(Player *p);
void settings(Player *p, Game *g);
void profile(Player *p);
void game_launcher(Player *p, Game *g);
void map_builder(Game *g, int initial_room);
int handle_movement(chtype **screen, int **visited, int ch, Game *g);
void display_health(Game *g);
void corridor_path(char direction, Pos door1, Pos door2);
void display_screen(Game *g, char mode[], int **visited, chtype **screen);
void floor_generator(Player *p, Game *g);
int check_room(Room *rooms, int i, int j);
void display_message(Game *g, int floor, int score, int gold);
int check_trap(Room *rooms, int i, int j);
int check_secret_door(int count, Pos *secret_doors, int i, int j);
void terminate_game(int code, Player *p, Game *g);
void you_won_screen(Player *p);
void you_lost_screen(Player *p);
void save_game(Player *p, Game *g, chtype **screen, int **visited);
void saved_game_launcher(Player *p, Game *g);
void saved_floor_generator(Player *p, Game *g, chtype **screen, int **visited);
void save_screen();
void not_saved_screen();
void food_screen(Game *g);
void message_to_password(int password);
void password_screen(Game *g);
void weapon_screen(Game *g);
void spell_screen(Game *g);
void enchant_room(Player *p, Game *g);
void play_music(const char *filename);
void stop_music();
int check_monsters(Game *g, int i, int j);
int check_monsters_alive(Game *g, int i, int j);
void handle_movement_monsters(Game *g, chtype **screen);
void handle_monsters(Game *g, chtype **screen);
int is_next(Pos a, Pos b);
void handle_weapons(Game *g, char ch, chtype **screen);
void shoot_weapon(Game *g, char ch, chtype **screen);
int check_weapons(Pos *weapons, int i, int j);
void battle_room(char mode[], Player *p, Game *g);

int main() {
    initscr();
    noecho();
    curs_set(FALSE);
    keypad(stdscr, TRUE);
    start_color();

    srand(time(NULL));

    Player *p = (Player *) malloc(sizeof(Player));
    login(p);

    Game *g = (Game *) malloc(sizeof(Game));
    g->difficulty = 0; g->player_color = 0; g->play_music = 1;
    menu_handler(p,g);
    
    endwin();
    return 0;
}

char message[100] = "Welcome to the game!";

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
        mvprintw(LINES / 2 + 1, COLS / 2 - 15, "Your password is INCORRECT!");
        
        const char *options[] = {"Forgot your password?", "EXIT"};
        int choice = 0;
        
        while (1) {
            for (int i=0; i<2; i++) {
                if (i == choice)
                    attron(A_REVERSE);
                mvprintw(LINES / 2 + 2 + i, COLS / 2 - 15, "%s", options[i]);
                if (i == choice)
                    attroff(A_REVERSE);
            }

            int ch = getch();
            if (ch == KEY_UP && choice != 0)
                choice--;
            else if (ch == KEY_DOWN && choice != 1)
                choice++;
            else if (ch == 10)
                break;
        }

        switch (choice) {
            case 0:
                forgot_password(p);
                login(p);
                break;
            case 1:
                endwin();
                exit(0);
                break;
        }
    }
    else if(user_check==0 && pass_check==0) {
        mvprintw(LINES / 2 + 1, COLS / 2 - 15, "Your username and password are INCORRECT!");

        const char *options[] = {"Don't have an account? MAKE ONE", "Enter as Guest", "EXIT"};
        int choice = 0;
        while (1) {
            for (int i=0; i<3; i++) {
                if (i == choice)
                    attron(A_REVERSE);
                mvprintw(LINES / 2 + 2 + i, COLS / 2 - 15, "%s", options[i]);
                if (i == choice)
                    attroff(A_REVERSE);
            }

            int ch = getch();
            if (ch == KEY_UP && choice != 0)
                choice--;
            else if (ch == KEY_DOWN && choice != 2)
                choice++;
            else if (ch == 10)
                break;
        }

        switch (choice) {
            case 0:
                create_account(p);
                login(p);
                break;
            case 1:
                strcpy(p->username, "Guest");
                strcpy(p->password, "");
                strcpy(p->email, "");
                p->score = 0; p->gold = 0; p->count_games = 0;
                break;
            case 2:
                endwin();
                exit(0);
                break;
        }
    }
    clear();
}

void forgot_password(Player *p) {
    clear();
    init_pair(1, 8, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);

    while(1) {
        attron(COLOR_PAIR(1)); attron(A_REVERSE);
        mvprintw(1, 1, "FORGOT YOUR PASSWORD");
        attroff(COLOR_PAIR(1)); attroff(A_REVERSE);
        mvprintw(2, 1, "Enter your email to retrieve your password");
        
        curs_set(TRUE);
        echo();
        char email[50]; char password[50];
        mvprintw(4, 1, "Enter your email: ");
        clrtoeol(); refresh();
        getnstr(email, 50);
        curs_set(FALSE);
        noecho();
        FILE *fptr;
        fptr = fopen("accounts.txt", "r");
        char line[50]; char last_line[50]; int found = 0;
        while (fgets(line, 50, fptr) != NULL) {
            line[strcspn(line, "\n")] = '\0';
            if (check_email(line) && strcmp(email, line) == 0) {
                strcpy(password, last_line);
                found = 1;
                break;
            }
            strcpy(last_line, line);
        }
        fclose(fptr);

        mvprintw(5, 1, "password: ");
        if(found) {
            mvprintw(5, 11, "%s                                     ", password);
            char ch = getch();
            break;
        }
        else {
            attron(COLOR_PAIR(2));
            mvprintw(5, 11, "The email you entered is not valid.");
            attroff(COLOR_PAIR(2));
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
        if(strcmp(p->password,"generate") == 0) {
            noecho();
            char password[8];
            password[0] = 65 + rand()%26;
            for(int i=1; i<=4; i++) {
                password[i] = 97 + rand()%26;
            }
            password[5] = 33 + rand()%15;
            for(int i=6; i<8; i++) {
                password[i] = 48 + rand()%10;
            }
            strcpy(p->password,password);
            p->password[8] = 0;
            mvprintw(LINES / 2, COLS / 2 + 7, "%s", p->password);
            int ch = getch();
        }
        if(!check_password(p->password)) {
            attron(COLOR_PAIR(2));
            mvprintw(LINES / 2 - 4, COLS / 2 - 15, "Make sure to use uppercase, lowercase and digit values (at least 7 characters). Or type generate to get a random safe password");
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
            FILE *last_saved_game = fopen(txt_format(p->username),"r");
            if(last_saved_game) {
                saved_game_launcher(p,g);
            }
            else {
                not_saved_screen();
                menu_handler(p,g);
            }
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

    const char *options[] = {"Start a new Game", "Play your last Game (Press Q in the game to save it as the last game you played)", "Scoreboard", "Settings", "Profile"};
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

    char usernames[50][50]; int scores[50]; int golds[50]; int game_counts[50];
    int arr[50];
    for(int i=0; i<50; i++) {
        arr[i] = i;
    }
    int *ptr_arr = arr;

    FILE *fptr;
    fptr = fopen("accounts.txt","r");
    char line[50]; int i = 0; int count = -1;
    while(fgets(line, 50, fptr) != NULL) {
        line[strcspn(line,"\n")] = '\0';
        if(i%6 == 0) {
            count++;
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

    sort(ptr_arr, scores, count+1);
    
    int page = 0;
    while(1) {
        attron(COLOR_PAIR(1));
        mvprintw(1, 1, "SCORE BOARD");
        attroff(COLOR_PAIR(1));
        mvprintw(1, 20, "Press left or right keys to move between pages");
        mvprintw(3, 1, "RANK"); mvprintw(3, 10, "username"); mvprintw(3, 24, "score"); mvprintw(3, 35, "gold"); mvprintw(3, 44, "gamecounts");
        clrtobot();
        for(int i=0; i<10 && i+page*10<=count; i++) {
            if(page == 0) {
                attron(COLOR_PAIR(2));
                mvprintw(4, 66, "  .__.");
                mvprintw(5, 66, " (|  |)");
                mvprintw(6, 66, "  (  )");
                mvprintw(7, 66, "  _)(_");
                attroff(COLOR_PAIR(2));
                if(0<=i && i<=2) {
                    attron(A_ITALIC);
                    attron(COLOR_PAIR(3));
                    mvprintw(5+i, 58, "LEGEND");
                    attroff(COLOR_PAIR(3));
                    attron(COLOR_PAIR(2)); 
                }
                else{
                    attroff(COLOR_PAIR(2)); attroff(A_ITALIC);
                }
            }
            if(strcmp(p->username,usernames[arr[i+page*10]]) == 0) {
                mvprintw(5+i, 1, "-> %d", i+1+page*10); mvprintw(5+i, 10, "%s", usernames[arr[i+page*10]]); mvprintw(5+i, 24, "%d", scores[arr[i+page*10]]); mvprintw(5+i, 35, "%d", golds[arr[i+page*10]]); mvprintw(5+i, 44, "%d", game_counts[arr[i+page*10]]);
            }
            else {
                mvprintw(5+i, 1, "%d", i+1+page*10); mvprintw(5+i, 10, "%s", usernames[arr[i+page*10]]); mvprintw(5+i, 24, "%d", scores[arr[i+page*10]]); mvprintw(5+i, 35, "%d", golds[arr[i+page*10]]); mvprintw(5+i, 44, "%d", game_counts[arr[i+page*10]]);
            }
        }

        attron(A_ITALIC); mvprintw(16, 30, "Page: %d", page+1); attroff(A_ITALIC);

        int ch = getch();
        if(ch == KEY_RIGHT && page != 4) {
            page += 1;
        }
        else if(ch == KEY_LEFT && page != 0) {
            page -= 1;
        }
        else {
            break;
        }
    }

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

    mvprintw(12, 1, "Do you want music to be played when you enter themed rooms?");
    g->play_music = 1;
    const char *options[] = {"No", "Yes"};
    while (1) {
        for (int i=0; i<2; i++) {
            if (i == g->play_music)
                attron(A_REVERSE);
            mvprintw(13+i, 1, "%s", options[i]);
            if (i == g->play_music)
                attroff(A_REVERSE);
        }

        int ch = getch();
        if (ch == KEY_UP && g->play_music == 1)
            g->play_music = 0;
        else if (ch == KEY_DOWN && g->play_music == 0)
            g->play_music = 1;
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
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, 208, COLOR_BLACK); //Orange
    init_pair(8, 8, COLOR_BLACK); //Gray
    init_pair(9, 13, COLOR_BLACK); //Pink
    init_pair(10, 2, COLOR_BLACK); //Dark Green

    if(g->difficulty) {
        g->MAX_health = 5;
        g->hungriness_rate = 2;
    }
    else {
        g->MAX_health = 10;
        g->hungriness_rate = 1;
    }
    g->players_health = g->MAX_health;
    g->players_extra_health = 0;
    g->players_score = 0;
    g->players_gold = 0;
    g->floor_number = 1;
    g->players_hungriness = 0;
    g->players_ordinary_food = 0;
    g->players_weapon = -1;
    g->players_weapon_direction = 5;
    g->players_mace = 1;
    g->players_dagger = 0;
    g->players_magic_wand = 0;
    g->players_arrow = 0;
    g->players_sword = 0;
    g->players_health_potion = 0;
    g->players_speed_potion = 0;
    g->players_speed = 1;
    g->players_damage_potion = 0;
    g->start_time = time(NULL);
    g->password_start_time = time(NULL);
    g->players_steps = 0;
    g->players_message_step = 0;
    g->players_speed_step = -15;
    g->players_health_step = -20;
    g->players_damage_step = -5;
    g->players_damage_rate = 1;
    g->players_ancient_key = 0;
    g->players_broken_key = 0;
    g->not_grab = 0;

    p->count_games++;

    floor_generator(p, g);
    

    clear();
}

void save_game(Player *p, Game *g, chtype **screen, int **visited) {
    FILE *saved_game = fopen(txt_format(p->username),"w");
    if(saved_game) {
        fprintf(saved_game, "%d\n%d\n%d\n%d\n", g->difficulty, g->player_color, g->players_health, g->players_extra_health);
        fprintf(saved_game, "%d\n%d\n", g->players_score, g->players_gold);
        fprintf(saved_game, "%d\n", g->floor_number);
        fprintf(saved_game, "%d\n%d\n", g->players_hungriness, g->players_ordinary_food);
        fprintf(saved_game, "%d\n%d\n%d\n%d\n%d\n%d\n%d\n", g->players_weapon, g->players_weapon_direction, g->players_mace, g->players_dagger, g->players_magic_wand, g->players_arrow, g->players_sword);
        fprintf(saved_game, "%d\n%d\n%d\n%d\n", g->players_health_potion, g->players_speed_potion, g->players_speed, g->players_damage_potion);
        fprintf(saved_game, "%d\n%d\n%d\n%d\n%d\n", g->players_message_step, g->players_speed_step, g->players_health_step, g->players_damage_step, g->players_damage_rate);
        fprintf(saved_game, "%d\n%d\n", g->player_pos.x, g->player_pos.y);
        fprintf(saved_game, "%d\n%d\n", g->players_steps, g->k_lock);
        fprintf(saved_game, "%d\n%d\n", g->players_ancient_key, g->players_broken_key);
        fprintf(saved_game, "%d\n", g->play_music);

        for(int k=0; k<6; k++) {
            fprintf(saved_game, "%d\n", g->rooms[k].type);
            fprintf(saved_game, "%d\n", g->rooms[k].locked);
            fprintf(saved_game, "%d\n", g->rooms[k].password);
            fprintf(saved_game, "%d\n%d\n", g->rooms[k].room_pos.x, g->rooms[k].room_pos.y);
            fprintf(saved_game, "%d\n%d\n", g->rooms[k].room_size_v, g->rooms[k].room_size_h);
            fprintf(saved_game, "%d\n%d\n", g->rooms[k].gold, g->rooms[k].dark_gold);
            fprintf(saved_game, "%d\n%d\n", g->rooms[k].ordinary_food, g->rooms[k].potion);
        }
        for(int w=0; w<3; w++) {
            fprintf(saved_game, "%d\n", g->weapons[w].x);
            fprintf(saved_game, "%d\n", g->weapons[w].y);
        }
        
        for(int j=1; j<LINES; j++) {
            for(int i=0; i<COLS; i++) {
                fprintf(saved_game, "%c", screen[i][j]);
            }
            fprintf(saved_game, "\n");
        }
        for(int j=1; j<LINES; j++) {
            for(int i=0; i<COLS; i++) {
                fprintf(saved_game, "%d", visited[i][j]);
            }
            fprintf(saved_game, "\n");
        }
        fclose(saved_game);
    }
}

void saved_game_launcher(Player *p, Game *g) {
    clear();
    init_pair(0, COLOR_WHITE, COLOR_BLACK);
    init_pair(1, COLOR_BLUE, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_YELLOW, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(7, 208, COLOR_BLACK); //Orange
    init_pair(8, 8, COLOR_BLACK); //Gray
    init_pair(9, 13, COLOR_BLACK); //Pink
    init_pair(10, 2, COLOR_BLACK); //Dark Green

    FILE *saved_game = fopen(txt_format(p->username),"r");
    char num[10];
    fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->difficulty = str_to_num(num); fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->player_color = str_to_num(num);
    fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_health = str_to_num(num); fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_extra_health = str_to_num(num);
    fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_score = str_to_num(num); fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_gold = str_to_num(num);
    fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->floor_number = str_to_num(num); fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_hungriness = str_to_num(num);
    fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_ordinary_food = str_to_num(num); fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_weapon = str_to_num(num);
    fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_weapon_direction = str_to_num(num); fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_mace = str_to_num(num);
    fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_dagger = str_to_num(num); fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_magic_wand = str_to_num(num);
    fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_arrow = str_to_num(num); fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_sword = str_to_num(num);
    fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_health_potion = str_to_num(num); fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_speed_potion = str_to_num(num);
    fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_speed = str_to_num(num); fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_damage_potion = str_to_num(num);
    fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_message_step = str_to_num(num); fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_speed_step = str_to_num(num);
    fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_health_step = str_to_num(num); fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_damage_step = str_to_num(num);
    fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_damage_rate = str_to_num(num);
    fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->player_pos.x = str_to_num(num); fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->player_pos.y = str_to_num(num);
    fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_steps = str_to_num(num); fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->k_lock = str_to_num(num);
    fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_ancient_key = str_to_num(num); fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->players_broken_key = str_to_num(num);
    fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->play_music = str_to_num(num);
    
    for(int k=0; k<6; k++) {
        fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->rooms[k].type = str_to_num(num);
        fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->rooms[k].locked = str_to_num(num);
        fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->rooms[k].password = str_to_num(num);
        fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->rooms[k].room_pos.x = str_to_num(num); fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->rooms[k].room_pos.y = str_to_num(num);
        fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->rooms[k].room_size_v = str_to_num(num); fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->rooms[k].room_size_h = str_to_num(num);
        fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->rooms[k].gold = str_to_num(num); fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->rooms[k].dark_gold= str_to_num(num);
        fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->rooms[k].ordinary_food = str_to_num(num); fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->rooms[k].potion = str_to_num(num);
    }

    for(int w=0; w<3; w++) {
        fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->weapons[w].x = str_to_num(num);
        fgets(num, 10, saved_game); num[strcspn(num,"\n")] = 0; g->weapons[w].y = str_to_num(num);
    }

    chtype **screen = (chtype **) malloc(COLS*sizeof(chtype*));
    for(int i=0; i<COLS; i++) {
        *(screen+i) = (chtype *) malloc(LINES*sizeof(chtype));
    }

    int **visited = (int **) malloc(COLS*sizeof(int *));
    for(int i=0; i<COLS; i++) {
        *(visited+i) = (int *) calloc(LINES,sizeof(int));
    }

    for(int j=1; j<LINES; j++) {
        for(int i=0; i<COLS; i++) {
            screen[i][j] = fgetc(saved_game);
        }
        fgetc(saved_game);
    }
    for(int j=1; j<LINES; j++) {
        for(int i=0; i<COLS; i++) {
            char ch;
            visited[i][j] = fgetc(saved_game)-48;
        }
        fgetc(saved_game);
    }

    fclose(saved_game);

    if(g->difficulty) {
        g->MAX_health = 5;
        g->hungriness_rate = 2;
    }
    else {
        g->MAX_health = 10;
        g->hungriness_rate = 1;
    }

    g->start_time = time(NULL);
    g->password_start_time = time(NULL);
    g->not_grab = 0;

    saved_floor_generator(p, g, screen, visited);
    
    clear();
}

void saved_floor_generator(Player *p, Game *g, chtype **screen, int **visited) {
    srand(time(NULL));
    clear();

    if(g->floor_number == 1) {
        strcpy(message, "You're now in the first floor!");
        for(int k=0; k<5; k++) {
            if(g->rooms[k].type == 2) {
                g->rooms[k].monsters_count = 0;
            }
            else {
                g->rooms[k].monsters_count = rand() % 2;
                for(int i=0; i<g->rooms[k].monsters_count; i++) {
                    g->rooms[k].monsters[i].type = 1+rand()%2; g->rooms[k].monsters[i].alive = 1; g->rooms[k].monsters[i].on = 0; g->rooms[k].monsters[i].freeze = 0; g->rooms[k].monsters[i].room = k;
                    if(g->rooms[k].monsters[i].type == 1) {g->rooms[k].monsters[i].radius = 1; g->rooms[k].monsters[i].health = 5; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 0;}
                    else if(g->rooms[k].monsters[i].type == 2) {g->rooms[k].monsters[i].radius = 3; g->rooms[k].monsters[i].health = 10; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 3;}
                }
            }
        }
    }
    else if(g->floor_number == 2) {
        strcpy(message, "You entered a new floor!");
        for(int k=0; k<6; k++) {
            if(g->rooms[k].type == 2) {
                g->rooms[k].monsters_count = 0;
            }
            else {
                g->rooms[k].monsters_count = 1;
                for(int i=0; i<g->rooms[k].monsters_count; i++) {
                    g->rooms[k].monsters[i].type = 1+rand()%3; g->rooms[k].monsters[i].alive = 1; g->rooms[k].monsters[i].on = 0; g->rooms[k].monsters[i].freeze = 0; g->rooms[k].monsters[i].room = k;
                    if(g->rooms[k].monsters[i].type == 1) {g->rooms[k].monsters[i].radius = 1; g->rooms[k].monsters[i].health = 5; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 0;}
                    else if(g->rooms[k].monsters[i].type == 2) {g->rooms[k].monsters[i].radius = 3; g->rooms[k].monsters[i].health = 10; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 3;}
                    else if(g->rooms[k].monsters[i].type == 3) {g->rooms[k].monsters[i].radius = 4; g->rooms[k].monsters[i].health = 15; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 5;}
                }
            } 
        }     
    }
    else if(g->floor_number == 3) {
        strcpy(message, "You entered a new floor!");
        for(int k=0; k<6; k++) {
            g->rooms[k].monsters_count = 1+rand()%2;
            for(int i=0; i<g->rooms[k].monsters_count; i++) {
                g->rooms[k].monsters[i].type = 1+rand()%4; g->rooms[k].monsters[i].alive = 1; g->rooms[k].monsters[i].on = 0; g->rooms[k].monsters[i].freeze = 0; g->rooms[k].monsters[i].room = k;
                if(g->rooms[k].monsters[i].type == 1) {g->rooms[k].monsters[i].radius = 1; g->rooms[k].monsters[i].health = 5; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 0;}
                else if(g->rooms[k].monsters[i].type == 2) {g->rooms[k].monsters[i].radius = 3; g->rooms[k].monsters[i].health = 10; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 3;}
                else if(g->rooms[k].monsters[i].type == 3) {g->rooms[k].monsters[i].radius = 4; g->rooms[k].monsters[i].health = 15; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 5;}
                else if(g->rooms[k].monsters[i].type == 4) {g->rooms[k].monsters[i].radius = 5; g->rooms[k].monsters[i].health = 20; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 100;}
            }
        }
    }
    else if(g->floor_number == 4) {
        strcpy(message, "You're now in the last floor!");
        for(int k=0; k<6; k++) {
            if(g->rooms[k].type == 1) {
                g->rooms[k].monsters_count = 0;
            }
            else {
                g->rooms[k].monsters_count = 1+rand()%2;
                for(int i=0; i<g->rooms[k].monsters_count; i++) {
                    g->rooms[k].monsters[i].type = 1+rand()%5; g->rooms[k].monsters[i].alive = 1; g->rooms[k].monsters[i].on = 0; g->rooms[k].monsters[i].freeze = 0; g->rooms[k].monsters[i].room = k;
                    if(g->rooms[k].monsters[i].type == 1) {g->rooms[k].monsters[i].radius = 1; g->rooms[k].monsters[i].health = 5; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 0;}
                    else if(g->rooms[k].monsters[i].type == 2) {g->rooms[k].monsters[i].radius = 3; g->rooms[k].monsters[i].health = 10; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 3;}
                    else if(g->rooms[k].monsters[i].type == 3) {g->rooms[k].monsters[i].radius = 4; g->rooms[k].monsters[i].health = 15; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 5;}
                    else if(g->rooms[k].monsters[i].type == 4) {g->rooms[k].monsters[i].radius = 5; g->rooms[k].monsters[i].health = 20; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 100;}
                    else if(g->rooms[k].monsters[i].type == 5) {g->rooms[k].monsters[i].radius = 10; g->rooms[k].monsters[i].health = 30; g->rooms[k].monsters[i].damage = 2; g->rooms[k].monsters[i].haunt = 8;}
                }
            }
        }
    }

    for(int k=0; k<6; k++) {
        if(g->rooms[k].type == 1) {
            g->rooms[k].traps_count = 7+rand()%5;
        }
        else if(g->rooms[k].type == 2) {
            g->rooms[k].traps_count = 0;
        }
        else {
            g->rooms[k].traps_count = 1+rand()%3;
        } 
    }

    g->rooms[g->k_lock].password = password();  int k_secret_door_1 = rand()%6; int k_secret_door_2 = rand()%6; g->secret_doors_count = 0;
    for(int k=0; k<6; k++) {
        int trap = 0; int secret_door = 0;
        int monster = 0;
        for(int j=1; j<LINES; j++) {
            for(int i=0; i<COLS; i++) {
                if(abs(i-g->rooms[k].room_pos.x) < g->rooms[k].room_size_h && abs(j-g->rooms[k].room_pos.y) < g->rooms[k].room_size_v) {
                    if(g->rooms[k].type == 1) {
                        if(rand()%10 == 1 && trap<g->rooms[k].traps_count) {
                            g->rooms[k].traps[trap].x = i; g->rooms[k].traps[trap].y = j;
                            trap++;
                        }
                    }
                    else {
                        if(rand()%((g->rooms[k].room_size_h*g->rooms[k].room_size_v)) == 0 && trap<g->rooms[k].traps_count) {
                            g->rooms[k].traps[trap].x = i; g->rooms[k].traps[trap].y = j;
                            trap++;
                        }
                    }
                    
                    if(g->floor_number != 4) {
                        if((k == k_secret_door_1 || k == k_secret_door_2) && g->rooms[k].type != 2) {
                            if(rand()%((g->rooms[k].room_size_h*g->rooms[k].room_size_v)) == 0 && secret_door<1) {
                                g->secret_doors[g->secret_doors_count].x = i; g->secret_doors[g->secret_doors_count].y = j;
                                secret_door++;
                                g->secret_doors_count++;
                            }
                        }  
                    }
                    
                    if(rand()%((g->rooms[k].room_size_h*g->rooms[k].room_size_v)) == 0 && monster<g->rooms[k].monsters_count) {
                        g->rooms[k].monsters[monster].position.x = i; g->rooms[k].monsters[monster].position.y = j;
                        monster++;
                    }
                }
            }
        }
    }

    int display_whole = 0;
    display_screen(g,"hidden",visited,screen);
    while(1) {
        display_message(g, g->floor_number, g->players_score, g->players_gold);
        display_health(g);
        time_t current_time = time(NULL);
        double elapsed_time = difftime(current_time, g->start_time);
        double password_time = difftime(current_time, g->password_start_time);

        if(elapsed_time > 30) {
            if(g->players_hungriness == 0 && g->players_health != 10) {
                g->players_health += 1;
            }
            if(g->players_hungriness < 10) {
                g->players_hungriness += g->hungriness_rate;
            }
            g->start_time = time(NULL);
            if(g->players_hungriness >= 5) {
                g->players_health -= g->players_hungriness/5;
            }
        }
        if(g->players_steps - g->players_speed_step >= 15) {g->players_speed = 1; g->players_speed_step = -15;}
        if(g->players_steps - g->players_health_step >= 30) {g->players_extra_health = 0; g->players_health_step = -30;}
        if(g->players_steps - g->players_damage_step >= 10) {g->players_damage_rate = 1; g->players_damage_step = -10;}
        if(g->players_health + g->players_extra_health <= 0) {
            terminate_game(0, p, g);
        }
        if(display_whole%2 == 1) {
            display_screen(g,"view",visited,screen);
        }
        else {
            display_screen(g,"hidden",visited,screen);
        }
        attron(COLOR_PAIR(g->player_color));
        mvprintw(g->player_pos.y, g->player_pos.x, "@");
        attroff(COLOR_PAIR(g->player_color));

        int ch = getch();
        if(ch == 'q') {
            save_game(p,g,screen,visited);
            save_screen();
        }
        else if(ch == 'm') {
            display_whole++;
            continue;
        }
        else if(ch == 'e') {
            food_screen(g);
        }
        else if(ch == 'i') {
            weapon_screen(g);
        }
        else if(ch == 'p') {
            spell_screen(g);
        }
        else if(ch == 'g') {
            g->not_grab = 1;
        }

        if(g->rooms[check_room(g->rooms, g->player_pos.x, g->player_pos.y)].type == 2) {
            short red, green, blue;
            color_content(COLOR_WHITE, &red, &green, &blue);
            init_color(COLOR_WHITE, 0, 128 * 1000 / 255, 128 * 1000 / 255); // Teal
            play_music("TheDeathWaltz.mp3");
            strcpy(message, "You're now in the nightmare room!");
            while(g->rooms[check_room(g->rooms, g->player_pos.x, g->player_pos.y)].type == 2) {
                clear();
                display_message(g, g->floor_number, g->players_score, g->players_gold);
                display_health(g);

                display_screen(g,"nightmare",visited,screen);

                attron(COLOR_PAIR(g->player_color));
                mvprintw(g->player_pos.y, g->player_pos.x, "@");
                attroff(COLOR_PAIR(g->player_color));

                int ch = getch();
                handle_movement(screen, visited, ch, g);
            }
            init_color(COLOR_WHITE, red, green, blue);
            stop_music();
            clear();
        }

        handle_monsters(g, screen);
        handle_movement_monsters(g, screen);

        handle_weapons(g, ch, screen);

        display_health(g);

        for(int i = g->player_pos.x-1; i <= g->player_pos.x+1; i++) {
            for(int j = g->player_pos.y-1; j <= g->player_pos.y+1; j++) {
                if(ch == 's') {
                    if(check_trap(g->rooms,i,j)) {
                        screen[i][j] = '^';
                    }
                }
                if(check_secret_door(g->secret_doors_count,g->secret_doors,i,j)) {
                    screen[i][j] = '?';
                }
            }
        }
        switch(handle_movement(screen, visited, ch, g)) {
            case 1:
                g->floor_number += 1;
                floor_generator(p,g);
                break;
            case 2:
                if(rand()%10 == 9) {
                    g->rooms[g->k_lock].password = password();
                }
                message_to_password(g->rooms[g->k_lock].password);
                break;
            case 3:
                password_screen(g);
                break;
            case 4:
                g->rooms[0].room_pos.x = COLS/2; g->rooms[0].room_pos.y = LINES/2; g->rooms[0].room_size_v = 12; g->rooms[0].room_size_h = 35;
                for(int i=0; i<6; i++) {g->rooms[i].monsters_count = 0;}
                battle_room("treasure", p, g);
                break;
            case 5:
                Pos saved_pos; saved_pos.x = g->player_pos.x; saved_pos.y = g->player_pos.y; int saved_secret_doors_count = g->secret_doors_count; Pos saved_first_door; saved_first_door.x = g->secret_doors[0].x; saved_first_door.y = g->secret_doors[0].y;
                int monsters_count[6]; for(int i=0; i<6; i++) { monsters_count[i] = g->rooms[i].monsters_count; g->rooms[i].monsters_count = 0;}
                g->enchant_start_time = time(NULL);
                enchant_room(p, g);
                g->player_pos.x = saved_pos.x; g->player_pos.y = saved_pos.y; g->secret_doors_count = saved_secret_doors_count; g->secret_doors[0].x = saved_first_door.x; g->secret_doors[0].y = saved_first_door.y;
                for(int i=0; i<6; i++) { g->rooms[i].monsters_count = monsters_count[i];}
                break;
            case 6:
                saved_pos.x = g->player_pos.x; saved_pos.y = g->player_pos.y; Pos room_0_pos; room_0_pos.x = g->rooms[0].room_pos.x; room_0_pos.y = g->rooms[0].room_pos.y;
                int size_v = g->rooms[0].room_size_v; int size_h = g->rooms[0].room_size_h;
                g->rooms[0].room_pos.x = COLS/2; g->rooms[0].room_pos.y = LINES/2; g->rooms[0].room_size_v = 12; g->rooms[0].room_size_h = 35;
                for(int i=0; i<6; i++) { monsters_count[i] = g->rooms[i].monsters_count; g->rooms[i].monsters_count = 0;}
                battle_room("trap", p, g);
                g->player_pos.x = saved_pos.x; g->player_pos.y = saved_pos.y;
                g->rooms[0].room_pos.x = room_0_pos.x; g->rooms[0].room_pos.y = room_0_pos.y; g->rooms[0].room_size_v = size_v; g->rooms[0].room_size_h = size_h;
                for(int i=0; i<6; i++) { g->rooms[i].monsters_count = monsters_count[i];}
                break;             
        }
    }
}

void floor_generator(Player *p, Game *g) {
    srand(time(NULL));
    clear();

    for(int k=0; k<6; k++) {
        g->rooms[k].type = 0;
        g->rooms[k].locked = 0;
    }

    if(g->floor_number == 1) {
        strcpy(message, "You're now in the first floor!");
        map_builder(g,6);

        if(rand()%2) {
            int k_nightmare;
            do {
                k_nightmare = rand()%6;
            } while(k_nightmare == check_room(g->rooms, g->player_pos.x, g->player_pos.y));
            g->rooms[k_nightmare].type = 2;
        }
        for(int k=0; k<5; k++) {
            if(g->rooms[k].type == 2) {g->rooms[k].monsters_count = 0;}
            else {g->rooms[k].monsters_count = rand() % 2;}
            for(int i=0; i<g->rooms[k].monsters_count; i++) {
                g->rooms[k].monsters[i].type = 1+rand()%2; g->rooms[k].monsters[i].alive = 1; g->rooms[k].monsters[i].on = 0; g->rooms[k].monsters[i].freeze = 0; g->rooms[k].monsters[i].room = k;
                if(g->rooms[k].monsters[i].type == 1) {g->rooms[k].monsters[i].radius = 1; g->rooms[k].monsters[i].health = 5; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 0;}
                else if(g->rooms[k].monsters[i].type == 2) {g->rooms[k].monsters[i].radius = 3; g->rooms[k].monsters[i].health = 10; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 3;}
            }
        }
    }
    else if(g->floor_number == 2) {
        strcpy(message, "You entered a new floor!");
        map_builder(g,check_room(g->rooms,g->player_pos.x,g->player_pos.y));

        int k_nightmare;
        do {
            k_nightmare = rand()%6;
        } while(k_nightmare == check_room(g->rooms, g->player_pos.x, g->player_pos.y));
        g->rooms[k_nightmare].type = 2;

        for(int k=0; k<6; k++) {
            if(g->rooms[k].type == 2) {g->rooms[k].monsters_count = 0;}
            else {g->rooms[k].monsters_count = 1;}
            for(int i=0; i<g->rooms[k].monsters_count; i++) {
                g->rooms[k].monsters[i].type = 1+rand()%3; g->rooms[k].monsters[i].alive = 1; g->rooms[k].monsters[i].on = 0; g->rooms[k].monsters[i].freeze = 0; g->rooms[k].monsters[i].room = k;
                if(g->rooms[k].monsters[i].type == 1) {g->rooms[k].monsters[i].radius = 1; g->rooms[k].monsters[i].health = 5; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 0;}
                else if(g->rooms[k].monsters[i].type == 2) {g->rooms[k].monsters[i].radius = 3; g->rooms[k].monsters[i].health = 10; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 3;}
                else if(g->rooms[k].monsters[i].type == 3) {g->rooms[k].monsters[i].radius = 4; g->rooms[k].monsters[i].health = 15; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 5;}
            }
        }
    }
    else if(g->floor_number == 3) {
        strcpy(message, "You entered a new floor!");
        map_builder(g,check_room(g->rooms,g->player_pos.x,g->player_pos.y));
        for(int k=0; k<6; k++) {
            g->rooms[k].monsters_count = 1+rand()%2;
            for(int i=0; i<g->rooms[k].monsters_count; i++) {
                g->rooms[k].monsters[i].type = 1+rand()%4; g->rooms[k].monsters[i].alive = 1; g->rooms[k].monsters[i].on = 0; g->rooms[k].monsters[i].freeze = 0; g->rooms[k].monsters[i].room = k;
                if(g->rooms[k].monsters[i].type == 1) {g->rooms[k].monsters[i].radius = 1; g->rooms[k].monsters[i].health = 5; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 0;}
                else if(g->rooms[k].monsters[i].type == 2) {g->rooms[k].monsters[i].radius = 3; g->rooms[k].monsters[i].health = 10; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 3;}
                else if(g->rooms[k].monsters[i].type == 3) {g->rooms[k].monsters[i].radius = 4; g->rooms[k].monsters[i].health = 15; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 5;}
                else if(g->rooms[k].monsters[i].type == 4) {g->rooms[k].monsters[i].radius = 5; g->rooms[k].monsters[i].health = 20; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 100;}
            }
        }
    }
    else if(g->floor_number == 4) {
        strcpy(message, "You're now in the last floor!");
        map_builder(g,check_room(g->rooms,g->player_pos.x,g->player_pos.y));
        int count_treasure_rooms = 0; double avg_size = 0;
        for(int k=0; k<6; k++) { avg_size += (g->rooms[k].room_size_h+g->rooms[k].room_size_v); }
        avg_size /= 6;
        for(int k=0; k<6; k++) { 
            if(g->rooms[k].room_size_h+g->rooms[k].room_size_v > avg_size && k!=check_room(g->rooms,g->player_pos.x,g->player_pos.y)) {
                g->rooms[k].type = 1;
                count_treasure_rooms += 1;
            }
            if(count_treasure_rooms == 2) {
                break;
            }
        }
        for(int k=0; k<6; k++) {
            if(g->rooms[k].type == 1) {
                g->rooms[k].monsters_count = 0;
            }
            else {
                g->rooms[k].monsters_count = 1+rand()%2;
                for(int i=0; i<g->rooms[k].monsters_count; i++) {
                    g->rooms[k].monsters[i].type = 1+rand()%5; g->rooms[k].monsters[i].alive = 1; g->rooms[k].monsters[i].on = 0; g->rooms[k].monsters[i].freeze = 0; g->rooms[k].monsters[i].room = k;
                    if(g->rooms[k].monsters[i].type == 1) {g->rooms[k].monsters[i].radius = 1; g->rooms[k].monsters[i].health = 5; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 0;}
                    else if(g->rooms[k].monsters[i].type == 2) {g->rooms[k].monsters[i].radius = 3; g->rooms[k].monsters[i].health = 10; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 3;}
                    else if(g->rooms[k].monsters[i].type == 3) {g->rooms[k].monsters[i].radius = 4; g->rooms[k].monsters[i].health = 15; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 5;}
                    else if(g->rooms[k].monsters[i].type == 4) {g->rooms[k].monsters[i].radius = 5; g->rooms[k].monsters[i].health = 20; g->rooms[k].monsters[i].damage = 1; g->rooms[k].monsters[i].haunt = 100;}
                    else if(g->rooms[k].monsters[i].type == 5) {g->rooms[k].monsters[i].radius = 10; g->rooms[k].monsters[i].health = 30; g->rooms[k].monsters[i].damage = 2; g->rooms[k].monsters[i].haunt = 8;}
                }
            }
        }
    }

    for(int k=0; k<6; k++) {
        if(g->rooms[k].type == 1) {
            g->rooms[k].pillar_seed.x = g->rooms[k].room_pos.x + power(-1,rand()%2)*(rand()%(g->rooms[k].room_size_h-3));
            g->rooms[k].pillar_seed.y = g->rooms[k].room_pos.y + power(-1,rand()%2)*(rand()%(g->rooms[k].room_size_v-3));
            g->rooms[k].traps_count = 7+rand()%5;
            g->rooms[k].gold = 0;
            g->rooms[k].dark_gold = 10;
            g->rooms[k].ordinary_food = 0;
        }
        else if(g->rooms[k].type == 2) {
            g->rooms[k].pillar_seed.x = g->rooms[k].room_pos.x + power(-1,rand()%2)*(rand()%(g->rooms[k].room_size_h-3));
            g->rooms[k].pillar_seed.y = g->rooms[k].room_pos.y + power(-1,rand()%2)*(rand()%(g->rooms[k].room_size_v-3));
            g->rooms[k].traps_count = 0;
            g->rooms[k].gold = 1 + rand()%4;
            g->rooms[k].dark_gold = rand()%2 * rand()%2 * rand()%2;
            g->rooms[k].ordinary_food = 1 + rand()%2;
            g->rooms[k].potion = rand()%2;
        }
        else {
            g->rooms[k].pillar_seed.x = g->rooms[k].room_pos.x + power(-1,rand()%2)*(rand()%(g->rooms[k].room_size_h-3));
            g->rooms[k].pillar_seed.y = g->rooms[k].room_pos.y + power(-1,rand()%2)*(rand()%(g->rooms[k].room_size_v-3));
            g->rooms[k].traps_count = 1+rand()%3;
            g->rooms[k].gold = 1 + rand()%4;
            g->rooms[k].dark_gold = rand()%2 * rand()%2 * rand()%2;
            g->rooms[k].ordinary_food = 1 + rand()%2;
            g->rooms[k].potion = rand()%2;
        } 
    }

    for(int j=1; j<LINES; j++) {
        for(int i=0; i<COLS; i++) {
            for(int k=0; k<6; k++) {
                if(abs(j-g->rooms[k].room_pos.y) == g->rooms[k].room_size_v && abs(i-g->rooms[k].room_pos.x) <= g->rooms[k].room_size_h) {
                    if(g->rooms[k].type == 1) {attron(COLOR_PAIR(5));}
                    mvprintw(j, i, "-");
                    if(g->rooms[k].type == 1) {attroff(COLOR_PAIR(5));}
                }
                else if(abs(i-g->rooms[k].room_pos.x) == g->rooms[k].room_size_h && abs(j-g->rooms[k].room_pos.y) < g->rooms[k].room_size_v) {
                    if(g->rooms[k].type == 1) {attron(COLOR_PAIR(5));}
                    mvprintw(j, i, "|");
                    if(g->rooms[k].type == 1) {attroff(COLOR_PAIR(5));}
                }
                else if(abs(i-g->rooms[k].room_pos.x) < g->rooms[k].room_size_h && abs(j-g->rooms[k].room_pos.y) < g->rooms[k].room_size_v) {
                    if(g->rooms[k].type == 1) {attron(COLOR_PAIR(4));}
                    mvprintw(j, i, ".");
                    if(g->rooms[k].type == 1) {attroff(COLOR_PAIR(4));}
                }
            }
        }
    }

    int k_stair_1; do { k_stair_1 = rand()%6; } while(k_stair_1 == check_room(g->rooms,g->player_pos.x,g->player_pos.y) || g->rooms[k_stair_1].type == 2); int k_stair_2 = rand()%6; do { g->k_lock = rand()%6; } while(g->rooms[g->k_lock].type == 2); g->rooms[g->k_lock].locked = 1; g->rooms[g->k_lock].password = password(); int k_ancient_key = rand()%6;  int k_secret_door_1 = rand()%6; int k_secret_door_2 = rand()%6; g->secret_doors_count = 0;
    int k_dagger = rand()%6; int k_magic_wand = rand()%6; int k_arrow = rand()%6; int k_sword = rand()%6;
    for(int k=0; k<6; k++) {
        if(g->rooms[k].type == 1) {
            draw_trophy(5,g->rooms[k].room_pos.x+g->rooms[k].room_size_h, g->rooms[k].room_pos.y+g->rooms[k].room_size_v);
        }
        if(g->rooms[k].locked == 1) {
            if(rand()%2) {
                mvprintw(g->rooms[k].room_pos.y+g->rooms[k].room_size_v-1, g->rooms[k].room_pos.x+g->rooms[k].room_size_h-1, "&");
            }
            else {
                mvprintw(g->rooms[k].room_pos.y-g->rooms[k].room_size_v+1, g->rooms[k].room_pos.x-g->rooms[k].room_size_h+1, "&");
            }
        }
        int d1=1; int d2=1; int stairs = 0; int trap = 0; int gold = 0; int dark_gold = 0; int ordinary_food = 0; int secret_door = 0; int potion = 0;
        int dagger = 0; int magic_wand = 0; int arrow = 0; int sword = 0; int monster = 0; int ancient_key = 0; 
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
                        if(k == g->k_lock) {
                            mvprintw(new_j, i, "@"); d2 = 0;
                        }
                        else {
                            mvprintw(new_j, i, "+"); d2 = 0;
                        }
                        g->rooms[k].doors[0].x = i; g->rooms[k].doors[0].y = new_j;
                    }
                }
                else if(k == 1) {
                    if(g->rooms[k].room_pos.y-j == g->rooms[k].room_size_v && abs(i-g->rooms[k].room_pos.x) <= g->rooms[k].room_size_h && d1) {
                        int new_i = i+rand()%g->rooms[k].room_size_h;
                        if(k == g->k_lock) {
                            mvprintw(j, new_i, "@"); d1 = 0;
                        }
                        else {
                            mvprintw(j, new_i, "+"); d1 = 0;
                        }
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
                        if(k == g->k_lock) {
                            mvprintw(new_j, i, "@"); d2 = 0;
                        }
                        else {
                            mvprintw(new_j, i, "+"); d2 = 0;
                        }
                        g->rooms[k].doors[1].x = i; g->rooms[k].doors[1].y = new_j;
                    }
                }
                else if(k == 4) {
                    if(j-g->rooms[k].room_pos.y == g->rooms[k].room_size_v && abs(i-g->rooms[k].room_pos.x) <= g->rooms[k].room_size_h && d1) {
                        int new_i = i+rand()%g->rooms[k].room_size_h;
                        if(k == g->k_lock) {
                            mvprintw(j, new_i, "@"); d1 = 0;
                        }
                        else {
                            mvprintw(j, new_i, "+"); d1 = 0;
                        }
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
                        if (k == g->k_lock) {
                            mvprintw(new_j, i, "@"); d2 = 0;
                        }
                        else {
                            mvprintw(new_j, i, "+"); d2 = 0;
                        }
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
                    if(g->rooms[k].type == 1) {
                        if(rand()%10 == 1 && (mvinch(j,i) & A_CHARTEXT) == '.' && dark_gold<g->rooms[k].dark_gold) {
                            mvprintw(j,i,"$");
                            dark_gold++;
                        }
                        if(rand()%10 == 1 && (mvinch(j,i) & A_CHARTEXT) == '.' && trap<g->rooms[k].traps_count) {
                            g->rooms[k].traps[trap].x = i; g->rooms[k].traps[trap].y = j;
                            trap++;
                        }
                    }
                    else {
                        if(rand()%((g->rooms[k].room_size_h*g->rooms[k].room_size_v)) == 0 && (mvinch(j,i) & A_CHARTEXT)  == '.' && dark_gold<g->rooms[k].dark_gold) {
                            mvprintw(j,i,"$");
                            dark_gold++;
                        }
                        if(rand()%((g->rooms[k].room_size_h*g->rooms[k].room_size_v)) == 0 && (mvinch(j,i) & A_CHARTEXT) == '.' && trap<g->rooms[k].traps_count) {
                            g->rooms[k].traps[trap].x = i; g->rooms[k].traps[trap].y = j;
                            trap++;
                        }
                    }
                    
                    if(g->floor_number != 4) {
                        if((k == k_stair_1 || k == k_stair_2) && k!=check_room(g->rooms,g->player_pos.x,g->player_pos.y)) {
                            if(rand()%((g->rooms[k].room_size_h*g->rooms[k].room_size_v)) == 0 && (mvinch(j,i) & A_CHARTEXT) == '.' && stairs<1) {
                                mvprintw(j, i, "<");
                                stairs++;
                            }
                        }
                        if((k == k_secret_door_1 || k == k_secret_door_2) && g->rooms[k].type != 2) {
                            if(rand()%((g->rooms[k].room_size_h*g->rooms[k].room_size_v)) == 0 && (mvinch(j,i) & A_CHARTEXT) == '.' && secret_door<1) {
                                g->secret_doors[g->secret_doors_count].x = i; g->secret_doors[g->secret_doors_count].y = j;
                                secret_door++;
                                g->secret_doors_count++;
                            }
                        }  
                    }
                    else {
                        if(k == k_sword) {
                            if(rand()%((g->rooms[k].room_size_h*g->rooms[k].room_size_v)) == 0 && (mvinch(j,i) & A_CHARTEXT) == '.' && sword<1) {
                                attron(COLOR_PAIR(8)); mvprintw(j,i-1,"-"); mvprintw(j,i,")"); mvprintw(j,i+1,"="); mvprintw(j,i+2,"="); mvprintw(j,i+3,">"); attroff(COLOR_PAIR(8));
                                sword++;
                            }
                        }
                    }
                    if(k == k_ancient_key) {
                        if(rand()%((g->rooms[k].room_size_h*g->rooms[k].room_size_v)) == 0 && (mvinch(j,i) & A_CHARTEXT) == '.' && !check_trap(g->rooms,i,j) && !check_secret_door(g->secret_doors_count,g->secret_doors,i,j) && ancient_key<1) {
                            attron(COLOR_PAIR(4)); mvprintw(j,i,"]"); attroff(COLOR_PAIR(4));
                            ancient_key++;
                        }
                    }
                    if(k == k_dagger) {
                        if(rand()%((g->rooms[k].room_size_h*g->rooms[k].room_size_v)) == 0 && (mvinch(j,i) & A_CHARTEXT) == '.' && !check_trap(g->rooms,i,j) && !check_secret_door(g->secret_doors_count,g->secret_doors,i,j) && dagger<1) {
                            mvprintw(j,i,"~");
                            g->weapons[0].x = i;  g->weapons[0].y = j;
                            dagger++;
                        }
                    }
                    if(k == k_magic_wand) {
                        if(rand()%((g->rooms[k].room_size_h*g->rooms[k].room_size_v)) == 0 && (mvinch(j,i) & A_CHARTEXT) == '.' && !check_trap(g->rooms,i,j) && !check_secret_door(g->secret_doors_count,g->secret_doors,i,j) && magic_wand<1) {
                            attron(COLOR_PAIR(9)); mvprintw(j,i,"!"); attroff(COLOR_PAIR(9));
                            g->weapons[1].x = i;  g->weapons[1].y = j;
                            magic_wand++;
                        }
                    }
                    if(k == k_arrow) {
                        if(rand()%((g->rooms[k].room_size_h*g->rooms[k].room_size_v)) == 0 && (mvinch(j,i) & A_CHARTEXT) == '.' && !check_trap(g->rooms,i,j) && !check_secret_door(g->secret_doors_count,g->secret_doors,i,j) && arrow<1) {
                            mvprintw(j,i,"}");
                            g->weapons[2].x = i;  g->weapons[2].y = j;
                            arrow++;
                        }
                    }
                    if(rand()%((g->rooms[k].room_size_h*g->rooms[k].room_size_v)) == 0 && (mvinch(j,i) & A_CHARTEXT) == '.' && !check_trap(g->rooms,i,j) && !check_secret_door(g->secret_doors_count,g->secret_doors,i,j) && gold<g->rooms[k].gold) {
                        mvprintw(j,i,"o");
                        gold++;
                    }
                    
                    if(rand()%((g->rooms[k].room_size_h*g->rooms[k].room_size_v)) == 0 && (mvinch(j,i) & A_CHARTEXT) == '.' && !check_trap(g->rooms,i,j) && !check_secret_door(g->secret_doors_count,g->secret_doors,i,j) && ordinary_food<g->rooms[k].ordinary_food) {
                        mvprintw(j,i,"f");
                        ordinary_food++;
                    }
                    if(rand()%((g->rooms[k].room_size_h*g->rooms[k].room_size_v)) == 0 && (mvinch(j,i) & A_CHARTEXT) == '.' && monster<g->rooms[k].monsters_count) {
                        g->rooms[k].monsters[monster].position.x = i; g->rooms[k].monsters[monster].position.y = j;
                        monster++;
                    }
                    if(rand()%((g->rooms[k].room_size_h*g->rooms[k].room_size_v)) == 0 && (mvinch(j,i) & A_CHARTEXT) == '.' && !check_trap(g->rooms,i,j) && !check_secret_door(g->secret_doors_count,g->secret_doors,i,j) && potion<g->rooms[k].potion) {
                        int num = rand()%3;
                        if(num%3 == 0) {
                            attron(COLOR_PAIR(3));
                            mvprintw(j, i,"\"");
                            attroff(COLOR_PAIR(3));
                        }
                        else if(num%3 == 1) {
                            attron(COLOR_PAIR(6));
                            mvprintw(j, i,"*");
                            attroff(COLOR_PAIR(6));
                        }
                        else if(num%3 == 2) {
                            attron(COLOR_PAIR(2));
                            mvprintw(j, i,"'");
                            attroff(COLOR_PAIR(2));
                        }
                        potion++;
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

    chtype **screen = (chtype **) malloc(COLS*sizeof(chtype*));
    for(int i=0; i<COLS; i++) {
        *(screen+i) = (chtype *) malloc(LINES*sizeof(chtype));
    }
    for(int j=1; j<LINES; j++) {
        for(int i=0; i<COLS; i++) {
            chtype character = mvinch(j,i);
            screen[i][j] = character;
        }
    }
    clear();

    int **visited = (int **) malloc(COLS*sizeof(int *));
    for(int i=0; i<COLS; i++) {
        *(visited+i) = (int *) calloc(LINES,sizeof(int));
    }
    int k = check_room(g->rooms,g->player_pos.x,g->player_pos.y);
    for(int i=g->rooms[k].room_pos.x-g->rooms[k].room_size_h-2; i<=g->rooms[k].room_pos.x+g->rooms[k].room_size_h+2; i++) {
        for(int j=g->rooms[k].room_pos.y-g->rooms[k].room_size_v-2; j<=g->rooms[k].room_pos.y+g->rooms[k].room_size_v+2; j++) {
            if((k == 0 && i<g->rooms[k].room_pos.x-g->rooms[k].room_size_h) || (k == 0 && j<g->rooms[k].room_pos.y-g->rooms[k].room_size_v)) { continue; }
            if((k == 1 && i<g->rooms[k].room_pos.x-g->rooms[k].room_size_h) || (k == 1 && j>g->rooms[k].room_pos.y+g->rooms[k].room_size_v)) { continue; }
            if((k == 2 && j<g->rooms[k].room_pos.y-g->rooms[k].room_size_v) || (k == 3 && j>g->rooms[k].room_pos.y+g->rooms[k].room_size_v)) { continue; }
            if((k == 4 && i>g->rooms[k].room_pos.x+g->rooms[k].room_size_h) || (k == 4 && j<g->rooms[k].room_pos.y-g->rooms[k].room_size_v)) { continue; }
            if((k == 5 && i>g->rooms[k].room_pos.x+g->rooms[k].room_size_h) || (k == 5 && j>g->rooms[k].room_pos.y+g->rooms[k].room_size_v)) { continue; }
            
            visited[i][j] = 1;
        }
    }

    int display_whole = 0;
    display_screen(g,"hidden",visited,screen);
    while(1) {
        display_message(g, g->floor_number, g->players_score, g->players_gold);
        display_health(g);
        time_t current_time = time(NULL);
        double elapsed_time = difftime(current_time, g->start_time);
        double password_time = difftime(current_time, g->password_start_time);

        if(elapsed_time > 30) {
            if(g->players_hungriness == 0 && g->players_health != 10) {
                g->players_health += 1;
            }
            if(g->players_hungriness < 10) {
                g->players_hungriness += g->hungriness_rate;
            }
            g->start_time = time(NULL);
            if(g->players_hungriness >= 5) {
                g->players_health -= g->players_hungriness/5;
            }
        }
        if(g->players_steps - g->players_speed_step >= 15) {g->players_speed = 1; g->players_speed_step = -15;}
        if(g->players_steps - g->players_health_step >= 30) {g->players_extra_health = 0; g->players_health_step = -30;}
        if(g->players_steps - g->players_damage_step >= 10) {g->players_damage_rate = 1; g->players_damage_step = -10;}
        if(g->players_health + g->players_extra_health <= 0) {
            terminate_game(0, p, g);
        }
        if(display_whole%2 == 1) {
            display_screen(g,"view",visited,screen);
        }
        else {
            display_screen(g,"hidden",visited,screen);
        }
        attron(COLOR_PAIR(g->player_color));
        mvprintw(g->player_pos.y, g->player_pos.x, "@");
        attroff(COLOR_PAIR(g->player_color));

        int ch = getch();
        if(ch == 'q') {
            save_game(p,g,screen,visited);
            save_screen();
        }
        else if(ch == 'm') {
            display_whole++;
            continue;
        }
        else if(ch == 'e') {
            food_screen(g);
        }
        else if(ch == 'i') {
            weapon_screen(g);
        }
        else if(ch == 'p') {
            spell_screen(g);
        }
        else if(ch == 'g') {
            g->not_grab = 1;
        }

        handle_monsters(g, screen);
        handle_movement_monsters(g, screen);

        handle_weapons(g, ch, screen);

        display_health(g);

        for(int i = g->player_pos.x-1; i <= g->player_pos.x+1; i++) {
            for(int j = g->player_pos.y-1; j <= g->player_pos.y+1; j++) {
                if(ch == 's') {
                    if(check_trap(g->rooms,i,j)) {
                        screen[i][j] = '^';
                    }
                }
                if(check_secret_door(g->secret_doors_count,g->secret_doors,i,j)) {
                    screen[i][j] = '?';
                }
            }
        }

        if(g->rooms[check_room(g->rooms, g->player_pos.x, g->player_pos.y)].type == 2) {
            short red, green, blue;
            color_content(COLOR_WHITE, &red, &green, &blue);
            init_color(COLOR_WHITE, 0, 128 * 1000 / 255, 128 * 1000 / 255); // Teal
            play_music("TheDeathWaltz.mp3");
            strcpy(message, "You're now in the nightmare room!");
            while(g->rooms[check_room(g->rooms, g->player_pos.x, g->player_pos.y)].type == 2) {
                clear();
                display_message(g, g->floor_number, g->players_score, g->players_gold);
                display_health(g);

                display_screen(g,"nightmare",visited,screen);

                attron(COLOR_PAIR(g->player_color));
                mvprintw(g->player_pos.y, g->player_pos.x, "@");
                attroff(COLOR_PAIR(g->player_color));

                int ch = getch();
                handle_movement(screen, visited, ch, g);
            }
            init_color(COLOR_WHITE, red, green, blue);
            stop_music();
            clear();
        }

        switch(handle_movement(screen, visited, ch, g)) {
            case 1:
                g->floor_number += 1;
                floor_generator(p,g);
                break;
            case 2:
                if(rand()%10 == 9) {
                    g->rooms[g->k_lock].password = password();
                }
                message_to_password(g->rooms[g->k_lock].password);
                break;
            case 3:
                password_screen(g);
                break;
            case 4:
                g->rooms[0].room_pos.x = COLS/2; g->rooms[0].room_pos.y = LINES/2; g->rooms[0].room_size_v = 12; g->rooms[0].room_size_h = 35;
                for(int i=0; i<6; i++) {g->rooms[i].monsters_count = 0;}
                battle_room("treasure", p, g);
                break;
            case 5:
                Pos saved_pos; saved_pos.x = g->player_pos.x; saved_pos.y = g->player_pos.y; int saved_secret_doors_count = g->secret_doors_count; Pos saved_first_door; saved_first_door.x = g->secret_doors[0].x; saved_first_door.y = g->secret_doors[0].y;
                int monsters_count[6]; for(int i=0; i<6; i++) { monsters_count[i] = g->rooms[i].monsters_count; g->rooms[i].monsters_count = 0;}
                g->enchant_start_time = time(NULL);
                enchant_room(p, g);
                g->player_pos.x = saved_pos.x; g->player_pos.y = saved_pos.y; g->secret_doors_count = saved_secret_doors_count; g->secret_doors[0].x = saved_first_door.x; g->secret_doors[0].y = saved_first_door.y;
                for(int i=0; i<6; i++) { g->rooms[i].monsters_count = monsters_count[i];}
                break;
            case 6:
                saved_pos.x = g->player_pos.x; saved_pos.y = g->player_pos.y; Pos room_0_pos; room_0_pos.x = g->rooms[0].room_pos.x; room_0_pos.y = g->rooms[0].room_pos.y;
                int size_v = g->rooms[0].room_size_v; int size_h = g->rooms[0].room_size_h;
                g->rooms[0].room_pos.x = COLS/2; g->rooms[0].room_pos.y = LINES/2; g->rooms[0].room_size_v = 12; g->rooms[0].room_size_h = 35;
                for(int i=0; i<6; i++) { monsters_count[i] = g->rooms[i].monsters_count; g->rooms[i].monsters_count = 0;}
                battle_room("trap", p, g);
                g->player_pos.x = saved_pos.x; g->player_pos.y = saved_pos.y;
                g->rooms[0].room_pos.x = room_0_pos.x; g->rooms[0].room_pos.y = room_0_pos.y; g->rooms[0].room_size_v = size_v; g->rooms[0].room_size_h = size_h;
                for(int i=0; i<6; i++) { g->rooms[i].monsters_count = monsters_count[i];}
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

int handle_movement(chtype **screen, int **visited, int ch, Game *g) {
    int count_dots = 0;
    chtype character = mvinch(g->player_pos.y-1, g->player_pos.x) & A_CHARTEXT; if(character == '.') {count_dots++;}
    character = mvinch(g->player_pos.y+1, g->player_pos.x) & A_CHARTEXT; if(character == '.') {count_dots++;}
    character = mvinch(g->player_pos.y, g->player_pos.x-1) & A_CHARTEXT; if(character == '.') {count_dots++;}
    character = mvinch(g->player_pos.y, g->player_pos.x+1) & A_CHARTEXT; if(character == '.') {count_dots++;}
    if(count_dots >= 2) {
        mvprintw(g->player_pos.y, g->player_pos.x, ".");
    }
    else if(count_dots == 1) {
        mvprintw(g->player_pos.y, g->player_pos.x, "+");
    }
    else {
        mvprintw(g->player_pos.y, g->player_pos.x, "#");
        if(abs(g->player_pos.x-g->rooms[0].room_pos.x) <= g->rooms[0].room_size_h+2 && abs(g->player_pos.y-g->rooms[0].room_pos.y) <= g->rooms[0].room_size_v+2) {
            for(int i=g->rooms[0].room_pos.x-g->rooms[0].room_size_h; i<=g->rooms[0].room_pos.x+g->rooms[0].room_size_h+2; i++) {
                for(int j=g->rooms[0].room_pos.y-g->rooms[0].room_size_v; j<=g->rooms[0].room_pos.y+g->rooms[0].room_size_v+2; j++) {
                    visited[i][j] = 1;
                }
            }
        }
        else if(abs(g->player_pos.x-g->rooms[1].room_pos.x) <= g->rooms[1].room_size_h+2 && abs(g->player_pos.y-g->rooms[1].room_pos.y) <= g->rooms[1].room_size_v+2) {
            for(int i=g->rooms[1].room_pos.x-g->rooms[1].room_size_h; i<=g->rooms[1].room_pos.x+g->rooms[1].room_size_h+2; i++) {
                for(int j=g->rooms[1].room_pos.y-g->rooms[1].room_size_v-2; j<=g->rooms[1].room_pos.y+g->rooms[1].room_size_v; j++) {
                    visited[i][j] = 1;
                }
            }
        }
        else if(abs(g->player_pos.x-g->rooms[2].room_pos.x) <= g->rooms[2].room_size_h+2 && abs(g->player_pos.y-g->rooms[2].room_pos.y) <= g->rooms[2].room_size_v+2) {
            for(int i=g->rooms[2].room_pos.x-g->rooms[2].room_size_h-2; i<=g->rooms[2].room_pos.x+g->rooms[2].room_size_h+2; i++) {
                for(int j=g->rooms[2].room_pos.y-g->rooms[2].room_size_v; j<=g->rooms[2].room_pos.y+g->rooms[2].room_size_v+2; j++) {
                    visited[i][j] = 1;
                }
            }
        }
        else if(abs(g->player_pos.x-g->rooms[3].room_pos.x) <= g->rooms[3].room_size_h+2 && abs(g->player_pos.y-g->rooms[3].room_pos.y) <= g->rooms[3].room_size_v+2) {
            for(int i=g->rooms[3].room_pos.x-g->rooms[3].room_size_h-2; i<=g->rooms[3].room_pos.x+g->rooms[3].room_size_h+2; i++) {
                for(int j=g->rooms[3].room_pos.y-g->rooms[3].room_size_v-2; j<=g->rooms[3].room_pos.y+g->rooms[3].room_size_v; j++) {
                    visited[i][j] = 1;
                }
            }
        }
        else if(abs(g->player_pos.x-g->rooms[4].room_pos.x) <= g->rooms[4].room_size_h+2 && abs(g->player_pos.y-g->rooms[4].room_pos.y) <= g->rooms[4].room_size_v+2) {
            for(int i=g->rooms[4].room_pos.x-g->rooms[4].room_size_h-2; i<=g->rooms[4].room_pos.x+g->rooms[4].room_size_h; i++) {
                for(int j=g->rooms[4].room_pos.y-g->rooms[4].room_size_v; j<=g->rooms[4].room_pos.y+g->rooms[4].room_size_v+2; j++) {
                    visited[i][j] = 1;
                }
            }
        }
        else if(abs(g->player_pos.x-g->rooms[5].room_pos.x) <= g->rooms[5].room_size_h+2 && abs(g->player_pos.y-g->rooms[5].room_pos.y) <= g->rooms[5].room_size_v+2) {
            for(int i=g->rooms[5].room_pos.x-g->rooms[5].room_size_h-2; i<=g->rooms[5].room_pos.x+g->rooms[5].room_size_h; i++) {
                for(int j=g->rooms[5].room_pos.y-g->rooms[5].room_size_v-2; j<=g->rooms[5].room_pos.y+g->rooms[5].room_size_v; j++) {
                    visited[i][j] = 1;
                }
            }
        }
        if(ch == '8' && mvinch(g->player_pos.y-1, g->player_pos.x) != ' ') {
            for(int j=1; j<=3; j++) {
                visited[g->player_pos.x][g->player_pos.y-j] = 1;
                visited[g->player_pos.x+1][g->player_pos.y-j] = 1;
                visited[g->player_pos.x-1][g->player_pos.y-j] = 1;
            }
        }
        else if(ch == '4' && mvinch(g->player_pos.y, g->player_pos.x-1) != ' ') {
            for(int i=1; i<=3; i++) {
                visited[g->player_pos.x-i][g->player_pos.y] = 1;
                visited[g->player_pos.x-i][g->player_pos.y+1] = 1;
                visited[g->player_pos.x-i][g->player_pos.y-1] = 1;
            }
        }
        else if(ch == '2' && mvinch(g->player_pos.y+1, g->player_pos.x) != ' ') {
            for(int j=1; j<=3; j++) {
                visited[g->player_pos.x][g->player_pos.y+j] = 1;
                visited[g->player_pos.x+1][g->player_pos.y+j] = 1;
                visited[g->player_pos.x-1][g->player_pos.y+j] = 1;
            }
        }
        else if(ch == '6' && mvinch(g->player_pos.y, g->player_pos.x+1) != ' ') {
            for(int i=1; i<=3; i++) {
                visited[g->player_pos.x+i][g->player_pos.y] = 1;
                visited[g->player_pos.x+i][g->player_pos.y-1] = 1;
                visited[g->player_pos.x+i][g->player_pos.y+1] = 1;
            }
        }
    }

    switch (ch) {
        case '8':
            character = mvinch(g->player_pos.y-g->players_speed, g->player_pos.x) & A_CHARTEXT;
            if(g->rooms[check_room(g->rooms, g->player_pos.x, g->player_pos.y)].type == 2 && character != '.' && character != '+' && character != '#'  && character != '-' && character != '|' && character != 'O') {character = '.';}
            g->players_steps += 1;
            if(character == '<') { return 1; }
            if(character == '_') { return 4; }
            if(character == 'o') { g->players_gold += 5; g->players_score +=10; screen[g->player_pos.x][g->player_pos.y-g->players_speed]='.'; g->player_pos.y -= g->players_speed; return 0; }
            if(character == '$') { g->players_gold += 25; g->players_score +=50; screen[g->player_pos.x][g->player_pos.y-g->players_speed]='.'; g->player_pos.y -= g->players_speed; return 0; }
            if(character == 'f') { g->players_ordinary_food += 1; screen[g->player_pos.x][g->player_pos.y-g->players_speed]='.'; g->player_pos.y -= g->players_speed; return 0; }
            if(check_trap(g->rooms,g->player_pos.x,g->player_pos.y-g->players_speed)) { screen[g->player_pos.x][g->player_pos.y-g->players_speed]='^'; if(rand()%3 == 0) {return 6;} else {g->players_health -= 1; return 0;}}
            if(check_secret_door(g->secret_doors_count,g->secret_doors,g->player_pos.x,g->player_pos.y-g->players_speed)) { screen[g->player_pos.x][g->player_pos.y-g->players_speed]='?'; return 5;}
            if(character == '&' && g->rooms[g->k_lock].locked == 1) { g->password_start_time = time(NULL); g->player_pos.y -= g->players_speed; return 2; }
            if(character == '@' && g->rooms[g->k_lock].locked == 1 && g->players_weapon == 5) { g->rooms[g->k_lock].locked = 0; if(rand()%10 == 1) {g->players_ancient_key -= 1; g->players_broken_key += 1;} return 0;}
            else if(character == '@' && g->rooms[g->k_lock].locked == 1 && g->players_weapon != 5) { return 3; }
            else if(character == '@' && g->rooms[g->k_lock].locked == 0) { g->player_pos.y -= g->players_speed; return 0; }
            if(!g->not_grab) {
                if (character == '.' || character == '+' || character == '#') { g->player_pos.y -= g->players_speed; }
                if(character == '"') { g->players_health_potion += 1; screen[g->player_pos.x][g->player_pos.y-g->players_speed]='.'; g->player_pos.y -= g->players_speed; return 0; }
                if(character == '*') { g->players_speed_potion += 1; screen[g->player_pos.x][g->player_pos.y-g->players_speed]='.'; g->player_pos.y -= g->players_speed; return 0; }
                if(character == '\'') { g->players_damage_potion += 1; screen[g->player_pos.x][g->player_pos.y-g->players_speed]='.'; g->player_pos.y -= g->players_speed; return 0; }
                if(character == '~') { if(check_weapons(g->weapons,g->player_pos.x,g->player_pos.y-g->players_speed)) {g->players_dagger += 10;} else {g->players_dagger += 1;} screen[g->player_pos.x][g->player_pos.y-g->players_speed]='.'; g->player_pos.y -= g->players_speed; return 0; }
                if(character == '!') { if(check_weapons(g->weapons,g->player_pos.x,g->player_pos.y-g->players_speed)) {g->players_magic_wand += 8;} else {g->players_magic_wand += 1;} screen[g->player_pos.x][g->player_pos.y-g->players_speed]='.'; g->player_pos.y -= g->players_speed; return 0; }
                if(character == '}') { if(check_weapons(g->weapons,g->player_pos.x,g->player_pos.y-g->players_speed)) {g->players_arrow += 20;} else {g->players_arrow += 1;} screen[g->player_pos.x][g->player_pos.y-g->players_speed]='.'; g->player_pos.y -= g->players_speed; return 0; }
                if(character == ']') { g->players_ancient_key += 1; screen[g->player_pos.x][g->player_pos.y-g->players_speed]='.'; g->player_pos.y -= g->players_speed; return 0; }
                if(character == ')') { g->players_sword += 1; for(int i=-g->players_speed; i<=3; i++) {screen[g->player_pos.x+i][g->player_pos.y-g->players_speed]='.';} g->player_pos.y -= g->players_speed; return 0; }
            }
            else {
                g->player_pos.y -= g->players_speed;
                g->not_grab = 0;
            }
            break;
        case '2':
            character = mvinch(g->player_pos.y+g->players_speed, g->player_pos.x) & A_CHARTEXT;
            if(g->rooms[check_room(g->rooms, g->player_pos.x, g->player_pos.y)].type == 2 && character != '.' && character != '+' && character != '#'  && character != '-' && character != '|' && character != 'O') {character = '.';}
            g->players_steps += 1;
            if(character == '<') { return 1; }
            if(character == '_') { return 4; }
            if(character == 'o') { g->players_gold += 5; g->players_score +=10; screen[g->player_pos.x][g->player_pos.y+g->players_speed]='.'; g->player_pos.y += g->players_speed; return 0; }
            if(character == '$') { g->players_gold += 25; g->players_score +=50; screen[g->player_pos.x][g->player_pos.y+g->players_speed]='.'; g->player_pos.y += g->players_speed; return 0; }
            if(character == 'f') { g->players_ordinary_food += 1; screen[g->player_pos.x][g->player_pos.y+g->players_speed]='.'; g->player_pos.y += g->players_speed; return 0; }
            if(check_trap(g->rooms,g->player_pos.x,g->player_pos.y+g->players_speed)) { screen[g->player_pos.x][g->player_pos.y+g->players_speed]='^'; if(rand()%3 == 0) {return 6;} else {g->players_health -= 1; return 0;}}
            if(check_secret_door(g->secret_doors_count,g->secret_doors,g->player_pos.x,g->player_pos.y+g->players_speed)) { screen[g->player_pos.x][g->player_pos.y+g->players_speed]='?'; return 5;}
            if(character == '&' && g->rooms[g->k_lock].locked == 1) { g->password_start_time = time(NULL); g->player_pos.y += g->players_speed; return 2; }
            if(character == '@' && g->rooms[g->k_lock].locked == 1 && g->players_weapon == 5) { g->rooms[g->k_lock].locked = 0; if(rand()%10 == 1) {g->players_ancient_key -= 1; g->players_broken_key += 1;} return 0;}
            else if(character == '@' && g->rooms[g->k_lock].locked == 1 && g->players_weapon != 5) { return 3; }
            else if(character == '@' && g->rooms[g->k_lock].locked == 0) { g->player_pos.y += g->players_speed; return 0; }
            if(!g->not_grab) {
                if (character == '.' || character == '+' || character == '#') { g->player_pos.y += g->players_speed; }
                if(character == '"') { g->players_health_potion += 1; screen[g->player_pos.x][g->player_pos.y+g->players_speed]='.'; g->player_pos.y += g->players_speed; return 0; }
                if(character == '*') { g->players_speed_potion += 1; screen[g->player_pos.x][g->player_pos.y+g->players_speed]='.'; g->player_pos.y += g->players_speed; return 0; }
                if(character == '\'') { g->players_damage_potion += 1; screen[g->player_pos.x][g->player_pos.y+g->players_speed]='.'; g->player_pos.y += g->players_speed; return 0; }
                if(character == '~') { if(check_weapons(g->weapons,g->player_pos.x,g->player_pos.y+g->players_speed)) {g->players_dagger += 10;} else {g->players_dagger += 1;} screen[g->player_pos.x][g->player_pos.y+g->players_speed]='.'; g->player_pos.y += g->players_speed; return 0; }
                if(character == '!') { if(check_weapons(g->weapons,g->player_pos.x,g->player_pos.y+g->players_speed)) {g->players_magic_wand += 8;} else {g->players_magic_wand += 1;} screen[g->player_pos.x][g->player_pos.y+g->players_speed]='.'; g->player_pos.y += g->players_speed; return 0; }
                if(character == '}') { if(check_weapons(g->weapons,g->player_pos.x,g->player_pos.y+g->players_speed)) {g->players_arrow += 20;} else {g->players_arrow += 1;} screen[g->player_pos.x][g->player_pos.y+g->players_speed]='.'; g->player_pos.y += g->players_speed; return 0; }
                if(character == ']') { g->players_ancient_key += 1; screen[g->player_pos.x][g->player_pos.y+g->players_speed]='.'; g->player_pos.y += g->players_speed; return 0; }
                if(character == ')') { g->players_sword += 1; for(int i=-g->players_speed; i<=3; i++) {screen[g->player_pos.x+i][g->player_pos.y+g->players_speed]='.';} g->player_pos.y += g->players_speed; return 0; }
            }
            else {
                g->player_pos.y += g->players_speed;
                g->not_grab = 0;
            }
            break;
        case '4':
            character = mvinch(g->player_pos.y, g->player_pos.x-g->players_speed) & A_CHARTEXT;
            if(g->rooms[check_room(g->rooms, g->player_pos.x, g->player_pos.y)].type == 2 && character != '.' && character != '+' && character != '#'  && character != '-' && character != '|' && character != 'O') {character = '.';}
            g->players_steps += 1;
            if(character == '<') { return 1; }
            if(character == '_') { return 4; }
            if(character == 'o') { g->players_gold += 5; g->players_score +=10; screen[g->player_pos.x-g->players_speed][g->player_pos.y]='.'; g->player_pos.x -= g->players_speed; return 0; }
            if(character == '$') { g->players_gold += 25; g->players_score +=50; screen[g->player_pos.x-g->players_speed][g->player_pos.y]='.'; g->player_pos.x -= g->players_speed; return 0; }
            if(character == 'f') { g->players_ordinary_food += 1; screen[g->player_pos.x-g->players_speed][g->player_pos.y]='.'; g->player_pos.x -= g->players_speed; return 0; }
            if(check_trap(g->rooms,g->player_pos.x-g->players_speed,g->player_pos.y)) { screen[g->player_pos.x-g->players_speed][g->player_pos.y]='^'; if(rand()%3 == 0) {return 6;} else {g->players_health -= 1; return 0;}}
            if(check_secret_door(g->secret_doors_count,g->secret_doors,g->player_pos.x-g->players_speed,g->player_pos.y)) { screen[g->player_pos.x-g->players_speed][g->player_pos.y]='?'; return 5;}
            if(character == '&' && g->rooms[g->k_lock].locked == 1) { g->password_start_time = time(NULL); g->player_pos.x -= g->players_speed; return 2; }
            if(character == '@' && g->rooms[g->k_lock].locked == 1 && g->players_weapon == 5) { g->rooms[g->k_lock].locked = 0; if(rand()%10 == 1) {g->players_ancient_key -= 1; g->players_broken_key += 1;} return 0;}
            else if(character == '@' && g->rooms[g->k_lock].locked == 1 && g->players_weapon != 5) { return 3; }
            else if(character == '@' && g->rooms[g->k_lock].locked == 0) { g->player_pos.x -= g->players_speed; return 0; }
            if(!g->not_grab) {
                if (character == '.' || character == '+' || character == '#') { g->player_pos.x -= g->players_speed; }
                if(character == '"') { g->players_health_potion += 1; screen[g->player_pos.x-g->players_speed][g->player_pos.y]='.'; g->player_pos.x -= g->players_speed; return 0; }
                if(character == '*') { g->players_speed_potion += 1; screen[g->player_pos.x-g->players_speed][g->player_pos.y]='.'; g->player_pos.x -= g->players_speed; return 0; }
                if(character == '\'') { g->players_damage_potion += 1; screen[g->player_pos.x-g->players_speed][g->player_pos.y]='.'; g->player_pos.x -= g->players_speed; return 0; }
                if(character == '~') { if(check_weapons(g->weapons,g->player_pos.x-g->players_speed,g->player_pos.y)) {g->players_dagger += 10;} else {g->players_dagger += 1;} screen[g->player_pos.x-g->players_speed][g->player_pos.y]='.'; g->player_pos.x -= g->players_speed; return 0; }
                if(character == '!') { if(check_weapons(g->weapons,g->player_pos.x-g->players_speed,g->player_pos.y)) {g->players_magic_wand += 8;} else {g->players_magic_wand += 1;} screen[g->player_pos.x-g->players_speed][g->player_pos.y]='.'; g->player_pos.x -= g->players_speed; return 0; }
                if(character == '}') { if(check_weapons(g->weapons,g->player_pos.x-g->players_speed,g->player_pos.y)) {g->players_arrow += 20;} else {g->players_arrow += 1;} screen[g->player_pos.x-g->players_speed][g->player_pos.y]='.'; g->player_pos.x -= g->players_speed; return 0; }
                if(character == ']') { g->players_ancient_key += 1; screen[g->player_pos.x-g->players_speed][g->player_pos.y]='.'; g->player_pos.x -= g->players_speed; return 0; }
            }
            else {
                g->player_pos.x -= g->players_speed;
                g->not_grab = 0;
            }
            break;
        case '6':
            character = mvinch(g->player_pos.y, g->player_pos.x+g->players_speed) & A_CHARTEXT;
            if(g->rooms[check_room(g->rooms, g->player_pos.x, g->player_pos.y)].type == 2 && character != '.' && character != '+' && character != '#'  && character != '-' && character != '|' && character != 'O') {character = '.';}
            g->players_steps += 1;
            if(character == '<') { return 1; }
            if(character == '_') { return 4; }
            if(character == 'o') { g->players_gold += 5; g->players_score +=10; screen[g->player_pos.x+g->players_speed][g->player_pos.y]='.'; g->player_pos.x += g->players_speed; return 0; }
            if(character == '$') { g->players_gold += 25; g->players_score +=50; screen[g->player_pos.x+g->players_speed][g->player_pos.y]='.'; g->player_pos.x += g->players_speed; return 0; }
            if(character == 'f') { g->players_ordinary_food += 1; screen[g->player_pos.x+g->players_speed][g->player_pos.y]='.'; g->player_pos.x += g->players_speed; return 0; }
            if(check_trap(g->rooms,g->player_pos.x+g->players_speed,g->player_pos.y)) { screen[g->player_pos.x+g->players_speed][g->player_pos.y]='^'; if(rand()%3 == 0) {return 6;} else {g->players_health -= 1; return 0;}}
            if(check_secret_door(g->secret_doors_count,g->secret_doors,g->player_pos.x+g->players_speed,g->player_pos.y)) { screen[g->player_pos.x+g->players_speed][g->player_pos.y]='?'; return 5;}
            if(character == '&' && g->rooms[g->k_lock].locked == 1) { g->password_start_time = time(NULL); g->player_pos.x += g->players_speed; return 2;}
            if(character == '@' && g->rooms[g->k_lock].locked == 1 && g->players_weapon == 5) { g->rooms[g->k_lock].locked = 0; if(rand()%10 == 1) {g->players_ancient_key -= 1; g->players_broken_key += 1;} return 0;}
            else if(character == '@' && g->rooms[g->k_lock].locked == 1 && g->players_weapon != 5) { return 3; }
            else if(character == '@' && g->rooms[g->k_lock].locked == 0) { g->player_pos.x += g->players_speed; return 0;}
            if(!g->not_grab) {
                if (character == '.' || character == '+' || character == '#') { g->player_pos.x += g->players_speed; }
                if(character == '"') { g->players_health_potion += 1; screen[g->player_pos.x+g->players_speed][g->player_pos.y]='.'; g->player_pos.x += g->players_speed; return 0; }
                if(character == '*') { g->players_speed_potion += 1; screen[g->player_pos.x+g->players_speed][g->player_pos.y]='.'; g->player_pos.x += g->players_speed; return 0; }
                if(character == '\'') { g->players_damage_potion += 1; screen[g->player_pos.x+g->players_speed][g->player_pos.y]='.'; g->player_pos.x += g->players_speed; return 0; }
                if(character == '~') { if(check_weapons(g->weapons,g->player_pos.x+g->players_speed,g->player_pos.y)) {g->players_dagger += 10;} else {g->players_dagger += 1;} screen[g->player_pos.x+g->players_speed][g->player_pos.y]='.'; g->player_pos.x += g->players_speed; return 0; }
                if(character == '!') { if(check_weapons(g->weapons,g->player_pos.x+g->players_speed,g->player_pos.y)) {g->players_magic_wand += 8;} else {g->players_magic_wand += 1;} screen[g->player_pos.x+g->players_speed][g->player_pos.y]='.'; g->player_pos.x += g->players_speed; return 0; }
                if(character == '}') { if(check_weapons(g->weapons,g->player_pos.x+g->players_speed,g->player_pos.y)) {g->players_arrow += 20;} else {g->players_arrow += 1;} screen[g->player_pos.x+g->players_speed][g->player_pos.y]='.'; g->player_pos.x += g->players_speed; return 0; }
                if(character == ']') { g->players_ancient_key += 1; screen[g->player_pos.x+g->players_speed][g->player_pos.y]='.'; g->player_pos.x += g->players_speed; return 0; }
            }
            else {
                g->player_pos.x += g->players_speed;
                g->not_grab = 0;
            }
            break;
        case '9':
            character = mvinch(g->player_pos.y-g->players_speed, g->player_pos.x+g->players_speed) & A_CHARTEXT;
            if(g->rooms[check_room(g->rooms, g->player_pos.x, g->player_pos.y)].type == 2 && character != '.' && character != '+' && character != '#'  && character != '-' && character != '|' && character != 'O') {character = '.';}
            g->players_steps += 1;
            if(character == '<') { return 1; }
            if(character == '_') { return 4; }
            if(character == 'o') { g->players_gold += 5; g->players_score +=10; screen[g->player_pos.x+g->players_speed][g->player_pos.y-g->players_speed]='.'; g->player_pos.y -= g->players_speed; g->player_pos.x += g->players_speed; return 0; }
            if(character == '$') { g->players_gold += 25; g->players_score +=50; screen[g->player_pos.x+g->players_speed][g->player_pos.y-g->players_speed]='.'; g->player_pos.y -= g->players_speed; g->player_pos.x += g->players_speed; return 0; }
            if(character == 'f') { g->players_ordinary_food += 1; screen[g->player_pos.x+g->players_speed][g->player_pos.y-g->players_speed]='.'; g->player_pos.y -= g->players_speed; g->player_pos.x += g->players_speed; return 0; }
            if(check_trap(g->rooms,g->player_pos.x+g->players_speed,g->player_pos.y-g->players_speed)) { screen[g->player_pos.x+g->players_speed][g->player_pos.y-g->players_speed]='^'; if(rand()%3 == 0) {return 6;} else {g->players_health -= 1; return 0;}}
            if(check_secret_door(g->secret_doors_count,g->secret_doors,g->player_pos.x+g->players_speed,g->player_pos.y-g->players_speed)) { screen[g->player_pos.x+g->players_speed][g->player_pos.y-g->players_speed]='?'; return 5;}
            if(character == '&' && g->rooms[g->k_lock].locked == 1) { g->password_start_time = time(NULL); g->player_pos.x += g->players_speed; g->player_pos.y -= g->players_speed; return 2;}
            if(character == '@' && g->rooms[g->k_lock].locked == 1 && g->players_weapon == 5) { g->rooms[g->k_lock].locked = 0; if(rand()%10 == 1) {g->players_ancient_key -= 1; g->players_broken_key += 1;} return 0;}
            else if(character == '@' && g->rooms[g->k_lock].locked == 1 && g->players_weapon != 5) { return 3; }
            else if(character == '@' && g->rooms[g->k_lock].locked == 0) { g->player_pos.x += g->players_speed; g->player_pos.y -= g->players_speed; return 0;}
            if(!g->not_grab) {
                if (character == '.' || character == '+' || character == '#') { g->player_pos.y -= g->players_speed; g->player_pos.x += g->players_speed; }
                if(character == '"') { g->players_health_potion += 1; screen[g->player_pos.x+g->players_speed][g->player_pos.y-g->players_speed]='.'; g->player_pos.y -= g->players_speed; g->player_pos.x += g->players_speed; return 0; }
                if(character == '*') { g->players_speed_potion += 1; screen[g->player_pos.x+g->players_speed][g->player_pos.y-g->players_speed]='.'; g->player_pos.y -= g->players_speed; g->player_pos.x += g->players_speed; return 0; }
                if(character == '\'') { g->players_damage_potion += 1; screen[g->player_pos.x+g->players_speed][g->player_pos.y-g->players_speed]='.'; g->player_pos.y -= g->players_speed; g->player_pos.x += g->players_speed; return 0; }
                if(character == '~') { if(check_weapons(g->weapons,g->player_pos.x+g->players_speed,g->player_pos.y-g->players_speed)) {g->players_dagger += 10;} else {g->players_dagger += 1;} screen[g->player_pos.x+g->players_speed][g->player_pos.y-g->players_speed]='.'; g->player_pos.x += g->players_speed; g->player_pos.y -= g->players_speed; return 0; }
                if(character == '!') { if(check_weapons(g->weapons,g->player_pos.x+g->players_speed,g->player_pos.y-g->players_speed)) {g->players_magic_wand += 8;} else {g->players_magic_wand += 1;} screen[g->player_pos.x+g->players_speed][g->player_pos.y-g->players_speed]='.'; g->player_pos.x += g->players_speed; g->player_pos.y -= g->players_speed; return 0; }
                if(character == '}') { if(check_weapons(g->weapons,g->player_pos.x+g->players_speed,g->player_pos.y-g->players_speed)) {g->players_arrow += 20;} else {g->players_arrow += 1;} screen[g->player_pos.x+g->players_speed][g->player_pos.y-g->players_speed]='.'; g->player_pos.x += g->players_speed; g->player_pos.y -= g->players_speed; return 0; }
                if(character == ']') { g->players_ancient_key += 1; screen[g->player_pos.x+g->players_speed][g->player_pos.y-g->players_speed]='.'; g->player_pos.y -= g->players_speed; g->player_pos.x += g->players_speed; return 0; }
                if(character == ')') { g->players_sword += 1; for(int i=-g->players_speed; i<=3; i++) {screen[g->player_pos.x+g->players_speed+i][g->player_pos.y-g->players_speed]='.';} g->player_pos.x += g->players_speed; g->player_pos.y -= g->players_speed; return 0; }
            }
            else {
                g->player_pos.y -= g->players_speed; g->player_pos.x += g->players_speed;
                g->not_grab = 0;
            }
            break;
        case '7':
            character = mvinch(g->player_pos.y-g->players_speed, g->player_pos.x-g->players_speed) & A_CHARTEXT;
            if(g->rooms[check_room(g->rooms, g->player_pos.x, g->player_pos.y)].type == 2 && character != '.' && character != '+' && character != '#'  && character != '-' && character != '|' && character != 'O') {character = '.';}
            g->players_steps += 1;
            if(character == '<') { return 1; }
            if(character == '_') { return 4; }
            if(character == 'o') { g->players_gold += 5; g->players_score +=10; screen[g->player_pos.x-g->players_speed][g->player_pos.y-g->players_speed]='.'; g->player_pos.y -= g->players_speed; g->player_pos.x -= g->players_speed; return 0; }
            if(character == '$') { g->players_gold += 25; g->players_score +=50; screen[g->player_pos.x-g->players_speed][g->player_pos.y-g->players_speed]='.'; g->player_pos.y -= g->players_speed; g->player_pos.x -= g->players_speed; return 0; }
            if(character == 'f') { g->players_ordinary_food += 1; screen[g->player_pos.x-g->players_speed][g->player_pos.y-g->players_speed]='.'; g->player_pos.y -= g->players_speed; g->player_pos.x -= g->players_speed; return 0; }
            if(check_trap(g->rooms,g->player_pos.x-g->players_speed,g->player_pos.y-g->players_speed)) { screen[g->player_pos.x-g->players_speed][g->player_pos.y-g->players_speed]='^'; if(rand()%3 == 0) {return 6;} else {g->players_health -= 1; return 0;}}
            if(check_secret_door(g->secret_doors_count,g->secret_doors,g->player_pos.x-g->players_speed,g->player_pos.y-g->players_speed)) { screen[g->player_pos.x-g->players_speed][g->player_pos.y-g->players_speed]='?'; return 5;}
            if(character == '&' && g->rooms[g->k_lock].locked == 1) { g->password_start_time = time(NULL); g->player_pos.x -= g->players_speed; g->player_pos.y -= g->players_speed; return 2;}
            if(character == '@' && g->rooms[g->k_lock].locked == 1 && g->players_weapon == 5) { g->rooms[g->k_lock].locked = 0; if(rand()%10 == 1) {g->players_ancient_key -= 1; g->players_broken_key += 1;} return 0;}
            else if(character == '@' && g->rooms[g->k_lock].locked == 1 && g->players_weapon != 5) { return 3; }
            else if(character == '@' && g->rooms[g->k_lock].locked == 0) { g->player_pos.x -= g->players_speed; g->player_pos.y -= g->players_speed; return 0;}
            if(!g->not_grab) {
                if (character == '.' || character == '+' || character == '#') { g->player_pos.y -= g->players_speed; g->player_pos.x -= g->players_speed; }
                if(character == '"') { g->players_health_potion += 1; screen[g->player_pos.x-g->players_speed][g->player_pos.y-g->players_speed]='.'; g->player_pos.y -= g->players_speed; g->player_pos.x -= g->players_speed; return 0; }
                if(character == '*') { g->players_speed_potion += 1; screen[g->player_pos.x-g->players_speed][g->player_pos.y-g->players_speed]='.'; g->player_pos.y -= g->players_speed; g->player_pos.x -= g->players_speed; return 0; }
                if(character == '\'') { g->players_damage_potion += 1; screen[g->player_pos.x-g->players_speed][g->player_pos.y-g->players_speed]='.'; g->player_pos.y -= g->players_speed; g->player_pos.x -= g->players_speed; return 0; }
                if(character == '~') { if(check_weapons(g->weapons,g->player_pos.x-g->players_speed,g->player_pos.y-g->players_speed)) {g->players_dagger += 10;} else {g->players_dagger += 1;} screen[g->player_pos.x-g->players_speed][g->player_pos.y-g->players_speed]='.'; g->player_pos.x -= g->players_speed; g->player_pos.y -= g->players_speed; return 0; }
                if(character == '!') { if(check_weapons(g->weapons,g->player_pos.x-g->players_speed,g->player_pos.y-g->players_speed)) {g->players_magic_wand += 8;} else {g->players_magic_wand += 1;} screen[g->player_pos.x-g->players_speed][g->player_pos.y-g->players_speed]='.'; g->player_pos.x -= g->players_speed; g->player_pos.y -= g->players_speed; return 0; }
                if(character == '}') { if(check_weapons(g->weapons,g->player_pos.x-g->players_speed,g->player_pos.y-g->players_speed)) {g->players_arrow += 20;} else {g->players_arrow += 1;} screen[g->player_pos.x-g->players_speed][g->player_pos.y-g->players_speed]='.'; g->player_pos.x -= g->players_speed; g->player_pos.y -= g->players_speed; return 0; }
                if(character == ']') { g->players_ancient_key += 1; screen[g->player_pos.x-g->players_speed][g->player_pos.y-g->players_speed]='.'; g->player_pos.y -= g->players_speed; g->player_pos.x -= g->players_speed; return 0; }
                if(character == ')') { g->players_sword += 1; for(int i=-g->players_speed; i<=3; i++) {screen[g->player_pos.x-g->players_speed+i][g->player_pos.y-g->players_speed]='.';} g->player_pos.x -= g->players_speed; g->player_pos.y -= g->players_speed; return 0; }
            }
            else {
                g->player_pos.y -= g->players_speed; g->player_pos.x -= g->players_speed;
                g->not_grab = 0;
            }
            break;
        case '3':
            character = mvinch(g->player_pos.y+g->players_speed, g->player_pos.x+g->players_speed) & A_CHARTEXT;
            if(g->rooms[check_room(g->rooms, g->player_pos.x, g->player_pos.y)].type == 2 && character != '.' && character != '+' && character != '#'  && character != '-' && character != '|' && character != 'O') {character = '.';}
            g->players_steps += 1;
            if(character == '<') { return 1; }
            if(character == '_') { return 4; }
            if(character == 'o') { g->players_gold += 5; g->players_score +=10; screen[g->player_pos.x+g->players_speed][g->player_pos.y+g->players_speed]='.'; g->player_pos.y += g->players_speed; g->player_pos.x += g->players_speed; return 0; }
            if(character == '$') { g->players_gold += 25; g->players_score +=50; screen[g->player_pos.x+g->players_speed][g->player_pos.y+g->players_speed]='.'; g->player_pos.y += g->players_speed; g->player_pos.x += g->players_speed; return 0; }
            if(character == 'f') { g->players_ordinary_food += 1; screen[g->player_pos.x+g->players_speed][g->player_pos.y+g->players_speed]='.'; g->player_pos.y += g->players_speed; g->player_pos.x += g->players_speed; return 0; }
            if(check_trap(g->rooms,g->player_pos.x+g->players_speed,g->player_pos.y+g->players_speed)) { screen[g->player_pos.x+g->players_speed][g->player_pos.y+g->players_speed]='^'; if(rand()%3 == 0) {return 6;} else {g->players_health -= 1; return 0;}}
            if(check_secret_door(g->secret_doors_count,g->secret_doors,g->player_pos.x+g->players_speed,g->player_pos.y+g->players_speed)) { screen[g->player_pos.x+g->players_speed][g->player_pos.y+g->players_speed]='?'; return 5;}
            if(character == '&' && g->rooms[g->k_lock].locked == 1) { g->password_start_time = time(NULL); g->player_pos.x += g->players_speed; g->player_pos.y += g->players_speed; return 2;}
            if(character == '@' && g->rooms[g->k_lock].locked == 1 && g->players_weapon == 5) { g->rooms[g->k_lock].locked = 0; if(rand()%10 == 1) {g->players_ancient_key -= 1; g->players_broken_key += 1;} return 0;}
            else if(character == '@' && g->rooms[g->k_lock].locked == 1 && g->players_weapon != 5) { return 3; }
            else if(character == '@' && g->rooms[g->k_lock].locked == 0) { g->player_pos.x += g->players_speed; g->player_pos.y += g->players_speed; return 0;}
            if(!g->not_grab) {
                if (character == '.' || character == '+' || character == '#') { g->player_pos.y += g->players_speed; g->player_pos.x += g->players_speed; }
                if(character == '"') { g->players_health_potion += 1; screen[g->player_pos.x+g->players_speed][g->player_pos.y+g->players_speed]='.'; g->player_pos.y += g->players_speed; g->player_pos.x += g->players_speed; return 0; }
                if(character == '*') { g->players_speed_potion += 1; screen[g->player_pos.x+g->players_speed][g->player_pos.y+g->players_speed]='.'; g->player_pos.y += g->players_speed; g->player_pos.x += g->players_speed; return 0; }
                if(character == '\'') { g->players_damage_potion += 1; screen[g->player_pos.x+g->players_speed][g->player_pos.y+g->players_speed]='.'; g->player_pos.y += g->players_speed; g->player_pos.x += g->players_speed; return 0; }
                if(character == '~') { if(check_weapons(g->weapons,g->player_pos.x+g->players_speed,g->player_pos.y+g->players_speed)) {g->players_dagger += 10;} else {g->players_dagger += 1;} screen[g->player_pos.x+g->players_speed][g->player_pos.y+g->players_speed]='.'; g->player_pos.x += g->players_speed; g->player_pos.y += g->players_speed; return 0; }
                if(character == '!') { if(check_weapons(g->weapons,g->player_pos.x+g->players_speed,g->player_pos.y+g->players_speed)) {g->players_magic_wand += 8;} else {g->players_magic_wand += 1;} screen[g->player_pos.x+g->players_speed][g->player_pos.y+g->players_speed]='.'; g->player_pos.x += g->players_speed; g->player_pos.y += g->players_speed; return 0; }
                if(character == '}') { if(check_weapons(g->weapons,g->player_pos.x+g->players_speed,g->player_pos.y+g->players_speed)) {g->players_arrow += 20;} else {g->players_arrow += 1;} screen[g->player_pos.x+g->players_speed][g->player_pos.y+g->players_speed]='.'; g->player_pos.x += g->players_speed; g->player_pos.y += g->players_speed; return 0; }
                if(character == ']') { g->players_ancient_key += 1; screen[g->player_pos.x+g->players_speed][g->player_pos.y+g->players_speed]='.'; g->player_pos.y += g->players_speed; g->player_pos.x += g->players_speed; return 0; }
                if(character == ')') { g->players_sword += 1; for(int i=-g->players_speed; i<=3; i++) {screen[g->player_pos.x+g->players_speed+i][g->player_pos.y+g->players_speed]='.';} g->player_pos.x += g->players_speed; g->player_pos.y += g->players_speed; return 0; }
            }
            else {
                g->player_pos.y += g->players_speed; g->player_pos.x += g->players_speed;
                g->not_grab = 0;
            }
            break;
        case '1':
            character = mvinch(g->player_pos.y+g->players_speed, g->player_pos.x-g->players_speed) & A_CHARTEXT;
            if(g->rooms[check_room(g->rooms, g->player_pos.x, g->player_pos.y)].type == 2 && character != '.' && character != '+' && character != '#'  && character != '-' && character != '|' && character != 'O') {character = '.';}
            g->players_steps += 1;
            if(g->rooms[check_room(g->rooms, g->player_pos.x, g->player_pos.y)].type == 2) { return 7; }
            if(character == '<') { return 1; }
            if(character == '_') { return 4; }
            if(character == 'o') { g->players_gold += 5; g->players_score +=10; screen[g->player_pos.x-g->players_speed][g->player_pos.y+g->players_speed]='.'; g->player_pos.y += g->players_speed; g->player_pos.x -= g->players_speed; return 0; }
            if(character == '$') { g->players_gold += 25; g->players_score +=50; screen[g->player_pos.x-g->players_speed][g->player_pos.y+g->players_speed]='.'; g->player_pos.y += g->players_speed; g->player_pos.x -= g->players_speed; return 0; }
            if(character == 'f') { g->players_ordinary_food += 1; screen[g->player_pos.x-g->players_speed][g->player_pos.y+g->players_speed]='.'; g->player_pos.y += g->players_speed; g->player_pos.x -= g->players_speed; return 0; }
            if(check_trap(g->rooms,g->player_pos.x-g->players_speed,g->player_pos.y+g->players_speed)) { screen[g->player_pos.x-g->players_speed][g->player_pos.y+g->players_speed]='^'; if(rand()%3 == 0) {return 6;} else {g->players_health -= 1; return 0;}}
            if(check_secret_door(g->secret_doors_count,g->secret_doors,g->player_pos.x-g->players_speed,g->player_pos.y+g->players_speed)) { screen[g->player_pos.x-g->players_speed][g->player_pos.y+g->players_speed]='?'; return 5;}
            if(character == '&' && g->rooms[g->k_lock].locked == 1) { g->password_start_time = time(NULL); g->player_pos.x -= g->players_speed; g->player_pos.y += g->players_speed; return 2;}
            if(character == '@' && g->rooms[g->k_lock].locked == 1 && g->players_weapon == 5) { g->rooms[g->k_lock].locked = 0; if(rand()%10 == 1) {g->players_ancient_key -= 1; g->players_broken_key += 1;} return 0;}
            else if(character == '@' && g->rooms[g->k_lock].locked == 1 && g->players_weapon != 5) { return 3; }
            else if(character == '@' && g->rooms[g->k_lock].locked == 0) { g->player_pos.x -= g->players_speed; g->player_pos.y += g->players_speed; return 0; }
            if(!g->not_grab) {
                if (character == '.' || character == '+' || character == '#') { g->player_pos.y += g->players_speed; g->player_pos.x -= g->players_speed; }
                if(character == '"') { g->players_health_potion += 1; screen[g->player_pos.x-g->players_speed][g->player_pos.y+g->players_speed]='.'; g->player_pos.y += g->players_speed; g->player_pos.x -= g->players_speed; return 0; }
                if(character == '*') { g->players_speed_potion += 1; screen[g->player_pos.x-g->players_speed][g->player_pos.y+g->players_speed]='.'; g->player_pos.y += g->players_speed; g->player_pos.x -= g->players_speed; return 0; }
                if(character == '\'') { g->players_damage_potion += 1; screen[g->player_pos.x-g->players_speed][g->player_pos.y+g->players_speed]='.'; g->player_pos.y += g->players_speed; g->player_pos.x -= g->players_speed; return 0; }
                if(character == '~') { if(check_weapons(g->weapons,g->player_pos.x-g->players_speed,g->player_pos.y+g->players_speed)) {g->players_dagger += 10;} else {g->players_dagger += 1;} screen[g->player_pos.x-g->players_speed][g->player_pos.y+g->players_speed]='.'; g->player_pos.x -= g->players_speed; g->player_pos.y += g->players_speed; return 0; }
                if(character == '!') { if(check_weapons(g->weapons,g->player_pos.x-g->players_speed,g->player_pos.y+g->players_speed)) {g->players_magic_wand += 8;} else {g->players_magic_wand += 1;} screen[g->player_pos.x-g->players_speed][g->player_pos.y+g->players_speed]='.'; g->player_pos.x -= g->players_speed; g->player_pos.y += g->players_speed; return 0; }
                if(character == '}') { if(check_weapons(g->weapons,g->player_pos.x-g->players_speed,g->player_pos.y+g->players_speed)) {g->players_arrow += 20;} else {g->players_arrow += 1;} screen[g->player_pos.x-g->players_speed][g->player_pos.y+g->players_speed]='.'; g->player_pos.x -= g->players_speed; g->player_pos.y += g->players_speed; return 0; }
                if(character == ']') { g->players_ancient_key += 1; screen[g->player_pos.x-g->players_speed][g->player_pos.y+g->players_speed]='.'; g->player_pos.y += g->players_speed; g->player_pos.x -= g->players_speed; return 0; }
                if(character == ')') { g->players_sword += 1; for(int i=-g->players_speed; i<=3; i++) {screen[g->player_pos.x-g->players_speed+i][g->player_pos.y+g->players_speed]='.';} g->player_pos.x -= g->players_speed; g->player_pos.y += g->players_speed; return 0; }
            }
            else {
                g->player_pos.y += g->players_speed; g->player_pos.x -= g->players_speed;
                g->not_grab = 0;
            }
            break;
        case 'f':
            ch = getch();
            if(ch == '8') {
                character = mvinch(g->player_pos.y-1, g->player_pos.x) & A_CHARTEXT;
                while(character == '.' || character == '+' || character == '#') {
                    g->players_steps += 1;
                    g->player_pos.y--;
                    character = mvinch(g->player_pos.y-1, g->player_pos.x) & A_CHARTEXT;
                }
            }
            else if(ch == '2') {
                character = mvinch(g->player_pos.y+1, g->player_pos.x) & A_CHARTEXT;
                while(character == '.' || character == '+' || character == '#') {
                    g->players_steps += 1;
                    g->player_pos.y++;
                    character = mvinch(g->player_pos.y+1, g->player_pos.x) & A_CHARTEXT;
                }
            }
            else if(ch == '4') {
                character = mvinch(g->player_pos.y, g->player_pos.x-1) & A_CHARTEXT;
                while(character == '.' || character == '+' || character == '#') {
                    g->players_steps += 1;
                    g->player_pos.x--;
                    character = mvinch(g->player_pos.y, g->player_pos.x-1) & A_CHARTEXT;
                }
            }
            else if(ch == '6') {
                character = mvinch(g->player_pos.y, g->player_pos.x+1) & A_CHARTEXT;
                while(character == '.' || character == '+' || character == '#') {
                    g->players_steps += 1;
                    g->player_pos.x++;
                    character = mvinch(g->player_pos.y, g->player_pos.x+1) & A_CHARTEXT;
                }
            }
            break;
    }
    return 0;
}

void display_health(Game *g) {
    mvprintw(0, COLS-2*g->MAX_health-10, "                              ");
    mvprintw(0, COLS-2*g->MAX_health-10, "Health: ");
    mvprintw(0, COLS-2*g->MAX_health-2, "[");
    int count_hearts = g->players_health;
    attron(COLOR_PAIR(2));
    int i=0;
    for(i; i<count_hearts; i++) {
        mvprintw(0, COLS-2*g->MAX_health-1+2*i, "<3");
    }
    attroff(COLOR_PAIR(2));
    int count_extra_hearts = g->players_extra_health;
    attron(COLOR_PAIR(3));
    int j = 0;
    for(i; j<count_extra_hearts; i++, j++) {
        mvprintw(0, COLS-2*g->MAX_health-1+2*i, "<3");
    }
    attroff(COLOR_PAIR(3));
    mvprintw(0, COLS-1, "]");

}

void corridor_path(char direction, Pos door1, Pos door2) {
    if(direction == 'v') {
        int num;
        if(door2.y-door1.y-1 != 0) {
            num = 1 + rand()%(door2.y-door1.y-1);
        }
        else {
            num = 1 + rand()%(door2.y-door1.y);
        } 
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
        int num;
        if(door2.x-door1.x-1 != 0) {
            num = 1 + rand()%(door2.x-door1.x-1);
        }
        else {
            num = 1 + rand()%(door2.x-door1.x);
        }
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

void display_screen(Game *g, char mode[], int **visited, chtype **screen) {
    if(strcmp(mode,"hidden") == 0) {
        for(int j=1; j<LINES; j++) {
            for(int i=0; i<COLS; i++) {
                if(visited[i][j]) {
                    if(check_monsters(g, i, j) && check_monsters_alive(g, i, j)) {
                        int type = check_monsters(g, i, j);
                        switch(type) {
                            case 1:
                                mvprintw(j,i,"D");
                                continue;
                            case 2:
                                attron(COLOR_PAIR(7));
                                mvprintw(j,i,"F");
                                attroff(COLOR_PAIR(7));
                                continue;
                            case 3:
                                mvprintw(j,i,"G");
                                continue;
                            case 4:
                                attron(COLOR_PAIR(10));
                                mvprintw(j,i,"S");
                                attroff(COLOR_PAIR(10));
                                continue;
                            case 5:
                                attron(COLOR_PAIR(8));
                                mvprintw(j,i,"U");
                                attroff(COLOR_PAIR(8));
                                continue;
                        }
                    }
                    if(screen[i][j] == '@' && g->rooms[check_room(g->rooms,i,j)].locked == 1) {
                        attron(COLOR_PAIR(2));
                        mvprintw(j,i,"%c",screen[i][j]);
                        attroff(COLOR_PAIR(2));
                        continue;
                    }
                    else if(screen[i][j] == '@' && g->rooms[check_room(g->rooms,i,j)].locked == 0) {
                        attron(COLOR_PAIR(3));
                        mvprintw(j,i,"%c",screen[i][j]);
                        attroff(COLOR_PAIR(3));
                        continue;
                    }
                    else if(screen[i][j] == '&' || screen[i][j] == '*') {
                        attron(COLOR_PAIR(6));
                        mvprintw(j,i,"%c",screen[i][j]);
                        attroff(COLOR_PAIR(6));
                        continue;
                    }
                    else if(screen[i][j] == '^' || screen[i][j] == '\'') {
                        attron(COLOR_PAIR(2));
                        mvprintw(j,i,"%c",screen[i][j]);
                        attroff(COLOR_PAIR(2));
                        continue;
                    }
                    else if(screen[i][j] == 'o' || screen[i][j] == ']') {
                        attron(COLOR_PAIR(4));
                        mvprintw(j,i,"%c",screen[i][j]);
                        attroff(COLOR_PAIR(4));
                        continue;
                    }
                    else if(screen[i][j] == '$' || screen[i][j] == '!') {
                        attron(COLOR_PAIR(5));
                        mvprintw(j,i,"%c",screen[i][j]);
                        attroff(COLOR_PAIR(5));
                        continue;
                    }
                    else if(screen[i][j] == 'f') {
                        attron(COLOR_PAIR(3)); attron(A_REVERSE);
                        mvprintw(j,i,"%c",screen[i][j]);
                        attroff(COLOR_PAIR(3)); attroff(A_REVERSE);
                        continue;
                    }
                    else if(screen[i][j] == '"') {
                        attron(COLOR_PAIR(10));
                        mvprintw(j,i,"%c",screen[i][j]);
                        attroff(COLOR_PAIR(10));
                        continue;
                    }
                    else if(screen[i][j] == '?') {
                        attron(COLOR_PAIR(9)); attron(A_REVERSE);
                        mvprintw(j,i,"%c",screen[i][j]);
                        attroff(COLOR_PAIR(9)); attroff(A_REVERSE);
                        continue;
                    }
                    if(g->rooms[check_room(g->rooms, i, j)].type == 1 && (screen[i][j] & A_CHARTEXT) == '.') {attron(COLOR_PAIR(4));}
                    else if(g->rooms[check_room(g->rooms, i, j)].type == 1 && ((screen[i][j] & A_CHARTEXT) == '|' || (screen[i][j] & A_CHARTEXT) == '-' || (screen[i][j] & A_CHARTEXT) == '_' || (screen[i][j] & A_CHARTEXT) == ')' || (screen[i][j] & A_CHARTEXT) == '(')) {attron(COLOR_PAIR(5));}
                    mvaddch(j,i,screen[i][j]);
                    if(g->rooms[check_room(g->rooms, i, j)].type == 1 && (screen[i][j] & A_CHARTEXT) == '.') {attroff(COLOR_PAIR(4));}
                    else if(g->rooms[check_room(g->rooms, i, j)].type == 1 && ((screen[i][j] & A_CHARTEXT) == '|' || (screen[i][j] & A_CHARTEXT) == '-' || (screen[i][j] & A_CHARTEXT) == '_' || (screen[i][j] & A_CHARTEXT) == ')' || (screen[i][j] & A_CHARTEXT) == '(')) {attroff(COLOR_PAIR(5));}
                }
                else {
                    mvprintw(j,i," ");
                }
            }
        }
    }
    else if(strcmp(mode,"nightmare") == 0) {
        for(int j=g->player_pos.y-2; j<=g->player_pos.y+2; j++) {
            for(int i=g->player_pos.x-2; i<=g->player_pos.x+2; i++) {
                mvprintw(j,i,"%c",screen[i][j]);
            }
        }
    }
    else {
        for(int j=1; j<LINES; j++) {
            for(int i=0; i<COLS; i++) {
                if(check_monsters(g, i, j) && check_monsters_alive(g, i, j)) {
                    int type = check_monsters(g, i, j);
                    switch(type) {
                        case 1:
                            mvprintw(j,i,"D");
                            continue;
                        case 2:
                            attron(COLOR_PAIR(7));
                            mvprintw(j,i,"F");
                            attroff(COLOR_PAIR(7));
                            continue;
                        case 3:
                            mvprintw(j,i,"G");
                            continue;
                        case 4:
                            attron(COLOR_PAIR(10));
                            mvprintw(j,i,"S");
                            attroff(COLOR_PAIR(10));
                            continue;
                        case 5:
                            attron(COLOR_PAIR(8));
                            mvprintw(j,i,"U");
                            attroff(COLOR_PAIR(8));
                            continue;
                    }
                }
                if(screen[i][j] == '@' && g->rooms[check_room(g->rooms,i,j)].locked == 1) {
                    attron(COLOR_PAIR(2));
                    mvprintw(j,i,"%c",screen[i][j]);
                    attroff(COLOR_PAIR(2));
                    continue;
                }
                else if(screen[i][j] == '@' && g->rooms[check_room(g->rooms,i,j)].locked == 0) {
                    attron(COLOR_PAIR(3));
                    mvprintw(j,i,"%c",screen[i][j]);
                    attroff(COLOR_PAIR(3));
                    continue;
                }
                else if(screen[i][j] == '&' || screen[i][j] == '*') {
                    attron(COLOR_PAIR(6));
                    mvprintw(j,i,"%c",screen[i][j]);
                    attroff(COLOR_PAIR(6));
                    continue;
                }
                else if(screen[i][j] == '^' || screen[i][j] == '\'') {
                    attron(COLOR_PAIR(2));
                    mvprintw(j,i,"%c",screen[i][j]);
                    attroff(COLOR_PAIR(2));
                    continue;
                }
                else if(screen[i][j] == 'o') {
                    attron(COLOR_PAIR(4));
                    mvprintw(j,i,"%c",screen[i][j]);
                    attroff(COLOR_PAIR(4));
                    continue;
                }
                else if(screen[i][j] == '$' || screen[i][j] == '!') {
                    attron(COLOR_PAIR(5));
                    mvprintw(j,i,"%c",screen[i][j]);
                    attroff(COLOR_PAIR(5));
                    continue;
                }
                else if(screen[i][j] == 'f') {
                    attron(COLOR_PAIR(3)); attron(A_REVERSE);
                    mvprintw(j,i,"%c",screen[i][j]);
                    attroff(COLOR_PAIR(3)); attroff(A_REVERSE);
                    continue;
                }
                else if(screen[i][j] == '"') {
                    attron(COLOR_PAIR(10));
                    mvprintw(j,i,"%c",screen[i][j]);
                    attroff(COLOR_PAIR(10));
                    continue;
                }
                else if(screen[i][j] == '?') {
                    attron(COLOR_PAIR(9)); attron(A_REVERSE);
                    mvprintw(j,i,"%c",screen[i][j]);
                    attroff(COLOR_PAIR(9)); attroff(A_REVERSE);
                    continue;
                }
                if(g->rooms[check_room(g->rooms, i, j)].type == 1 && (screen[i][j] & A_CHARTEXT) == '.') {attron(COLOR_PAIR(4));}
                else if(g->rooms[check_room(g->rooms, i, j)].type == 1 && ((screen[i][j] & A_CHARTEXT) == '|' || (screen[i][j] & A_CHARTEXT) == '-' || (screen[i][j] & A_CHARTEXT) == '_' || (screen[i][j] & A_CHARTEXT) == ')' || (screen[i][j] & A_CHARTEXT) == '(')) {attron(COLOR_PAIR(5));}
                mvaddch(j,i,screen[i][j]);
                if(g->rooms[check_room(g->rooms, i, j)].type == 1 && (screen[i][j] & A_CHARTEXT) == '.') {attroff(COLOR_PAIR(4));}
                else if(g->rooms[check_room(g->rooms, i, j)].type == 1 && ((screen[i][j] & A_CHARTEXT) == '|' || (screen[i][j] & A_CHARTEXT) == '-' || (screen[i][j] & A_CHARTEXT) == '_' || (screen[i][j] & A_CHARTEXT) == ')' || (screen[i][j] & A_CHARTEXT) == '(')) {attroff(COLOR_PAIR(5));}
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

void display_message(Game *g, int floor, int score, int gold) {
    mvprintw(0,0,"%s FLOOR:%d   SCORE:%d    GOLD:%d                         ", message, floor, score, gold);
    const char *messages[] = {"Welcome to the Game!", "Press e to view the food menu!", "Press i/p to view the weapons/spell menu!", "You can press s to locate traps around you!"};
    int step = g->players_steps - g->players_message_step;
    if(step%40 == 0) {
        int i = (step/40)%4;
        strcpy(message, messages[i]);
    }
}

int check_trap(Room *rooms, int i, int j) {
    for(int k=0; k<6; k++) {
        for(int l=0; l<rooms[k].traps_count; l++) {
            if(i == rooms[k].traps[l].x && j == rooms[k].traps[l].y) {
                return 1;
            }
        }
    }
    return 0;
}

int check_secret_door(int count, Pos *secret_doors, int i, int j) {
    for(int k=0; k<count; k++) {
        if(i == secret_doors[k].x && j == secret_doors[k].y) {
            return 1;
        }
    }
    return 0;
}

int check_monsters(Game *g, int i, int j) {
    for(int k=0; k<6; k++) {
        for(int m=0; m<g->rooms[k].monsters_count; m++) {
            if(i == g->rooms[k].monsters[m].position.x && j == g->rooms[k].monsters[m].position.y) {
                return g->rooms[k].monsters[m].type;
            }
        }
    }
    return 0;
}

int check_monsters_alive(Game *g, int i, int j) {
    for(int k=0; k<6; k++) {
        for(int m=0; m<g->rooms[k].monsters_count; m++) {
            if(i == g->rooms[k].monsters[m].position.x && j == g->rooms[k].monsters[m].position.y) {
                return g->rooms[k].monsters[m].alive;
            }
        }
    }
    return 0;
}

void terminate_game(int code, Player *p, Game *g) {
    p->score += g->players_score;
    p->gold += g->players_gold;
    FILE *accounts = fopen("accounts.txt", "r");
    FILE *temp_file = fopen("temp.txt", "w");
    char line[50];
    while(fgets(line, 50, accounts)) {
        line[strcspn(line,"\n")] = 0;
        if(strcmp(line,p->username) == 0) {
            fprintf(temp_file, "%s\n", line);
            fgets(line, 50, accounts); line[strcspn(line,"\n")] = 0; fprintf(temp_file, "%s\n", line);
            fgets(line, 50, accounts); line[strcspn(line,"\n")] = 0; fprintf(temp_file, "%s\n", line);
            fgets(line, 50, accounts); fprintf(temp_file, "%d\n", p->score);
            fgets(line, 50, accounts); fprintf(temp_file, "%d\n", p->gold);
            fgets(line, 50, accounts); fprintf(temp_file, "%d\n", p->count_games);
        }
        else {
            fprintf(temp_file, "%s\n", line);
        }
    }
    fclose(accounts);
    fclose(temp_file);
    remove("accounts.txt");
    rename("temp.txt","accounts.txt");
    if(code) {
        you_won_screen(p);
    }
    else {
        you_lost_screen(p);
    } 
}

void you_won_screen(Player *p) {
    clear();

    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);

    attron(COLOR_PAIR(2));
    draw_trophy(4, COLS/2-6, LINES/2+3);
    attroff(COLOR_PAIR(2));

    attron(COLOR_PAIR(1));
    mvprintw(LINES/2,COLS/2-4,"YOU WON!");
    attroff(COLOR_PAIR(1));

    mvprintw(LINES/2+2,COLS/2-4,"Press Q to EXIT or S to view the SCOREBOARD");

    int ch = getch();

    if(ch == 'q') {
        endwin();
        exit(0);
    }
    else if(ch == 's') {
        score_board(p);
    }
    you_won_screen(p);
}

void you_lost_screen(Player *p) {
    clear();

    init_pair(1, COLOR_BLACK, COLOR_RED);
    attron(COLOR_PAIR(1));
    mvprintw(LINES/2,COLS/2-4,"GAME OVER!");
    attroff(COLOR_PAIR(1));

    mvprintw(LINES/2+2,COLS/2-20,"Press Q to EXIT or S to view the SCOREBOARD");

    int ch = getch();

    if(ch == 'q') {
        endwin();
        exit(0);
    }
    else if(ch == 's') {
        score_board(p);
    }
    you_lost_screen(p);
}

void save_screen() {
    clear();

    init_pair(1, COLOR_BLACK, COLOR_GREEN);
    attron(COLOR_PAIR(1));
    mvprintw(LINES/2,COLS/2-16,"GAME HAS BEEN SAVED SUCCESSFULLY!");
    attroff(COLOR_PAIR(1));

    mvprintw(LINES/2+2,COLS/2-28,"Rerun the game to play your last game or start a new one");
    mvprintw(LINES/2+3,COLS/2-10,"Press any key to EXIT");

    int ch = getch();

    endwin();
    exit(0);
}

void not_saved_screen() {
    clear();

    mvprintw(LINES/2,COLS/2-14,"THERE ARE NO SAVED GAMES :(");
    mvprintw(LINES/2+2,COLS/2-14,"Press any key to RETURN");

    int ch = getch();
}

void food_screen(Game *g) {
    clear();

    attron(COLOR_PAIR(3));
    mvprintw(0, 1, "FOOD MENU");
    attroff(COLOR_PAIR(3));

    mvprintw(0, 20, "Choose a food to consume it");
    
    draw_cheese();

    const char *foods[] = {"Ordinary FOODS", "Super FOODS", "Magical FOODS", "Rotten FOODS", "RETURN"};

    int choose = 0;
    while (1) {
        display_health(g);
        mvprintw(10, 8, "                                           ");
        mvprintw(10, 8, "Hungriness: [");
        attron(COLOR_PAIR(2));
        for(int i=0; i<g->players_hungriness; i++) {
            mvprintw(10, 21+2*i, "**");
        }
        attroff(COLOR_PAIR(2));
        mvprintw(10, 41, "]");
        for (int i=0; i<5; i++) {
            if (i == choose)
                attron(A_REVERSE);
            if(i == 0) {
                mvprintw(12+i, 1, "%s (%d) ", foods[i], g->players_ordinary_food);
            }
            else {
                mvprintw(12+i, 1, "%s ", foods[i]);
            }
            if (i == choose)
                attroff(A_REVERSE);
        }

        int ch = getch();
        if (ch == KEY_UP && choose != 0)
            choose--;
        else if (ch == KEY_DOWN && choose != 4)
            choose++;
        else if (ch == 10) {
            if(choose == 0 && g->players_ordinary_food != 0 && g->players_hungriness > 0) {
                g->players_ordinary_food -= 1;
                g->players_hungriness -= 1;
            }
            else if(choose == 0 && g->players_ordinary_food != 0 && g->players_hungriness == 0 && g->players_health != g->MAX_health) {
                g->players_ordinary_food -= 1;
                g->players_health += 1;
            }
            if(choose == 4) {
                break;
            }
        }
        else if(ch != KEY_UP && ch != KEY_DOWN) {
            break;
        }
    }
    clear();
}

void weapon_screen(Game *g) {
    clear();

    draw_sword();

    attron(COLOR_PAIR(1));
    mvprintw(14, 1, "WEAPONS MENU");
    attroff(COLOR_PAIR(1));

    mvprintw(14, 20, "You can see weapons of the game here with their amount and description");
    
    char message[100] = " ";
    const char *weapons[] = {"Mace (z)", "-)==> Sword (x)", "~ Dagger (c)",  "! Magic Wand (v)", "} Normal Arrow (b)", "] Ancient Key (])", "Broken Key", message};
    

    int choose = g->players_weapon;

    attron(COLOR_PAIR(1));
    mvprintw(16, 1, "Short Range Weapons:");
    mvprintw(20, 1, "Ranged Weapons:");
    attroff(COLOR_PAIR(1));

    attron(COLOR_PAIR(4));
    mvprintw(16, 64, "Ancient Key:");
    attroff(COLOR_PAIR(4));

    while (1) {
        for (int i=0; i<8; i++) {
            if (i == choose)
                attron(A_REVERSE);
            if(i == 0) {
                mvprintw(17+i, 1, "%s [count: %d damage: 5]  ", weapons[i], g->players_mace);
            }
            else if(i == 1) {
                attron(COLOR_PAIR(8));
                mvprintw(17+i, 1, "%s [count: %d damage: 10]  ", weapons[i], g->players_sword);
                attroff(COLOR_PAIR(8));
            }
            else if(i == 2) {
                mvprintw(19+i, 1, "%s [count: %d damage: 12 range: 5]  ", weapons[i], g->players_dagger);
            }
            else if(i == 3) {
                attron(COLOR_PAIR(9));
                mvprintw(19+i, 1, "%s [count: %d damage: 15 range: 10]  ", weapons[i], g->players_magic_wand);
                attroff(COLOR_PAIR(9));
            }
            else if(i == 4) {
                mvprintw(19+i, 1, "%s [count: %d damage: 5 range: 5]  ", weapons[i], g->players_arrow);
            }
            else if(i == 5) {
                attron(COLOR_PAIR(4));
                mvprintw(12+i, 64, "%s [count: %d]  ", weapons[i], g->players_ancient_key);
                attroff(COLOR_PAIR(4));
            }
            else if(i == 6) {
                mvprintw(12+i, 64, "%s [count: %d]  ", weapons[i], g->players_broken_key);
            }
            else if(i == 7) {
                mvprintw(18+i, 1, "%s  ", weapons[i]);
            }
            if (i == choose)
                attroff(A_REVERSE);
        }

        int ch = getch();
        if (ch == 'w') {
            g->players_weapon = -1;
            choose = -1;
        }
        else if (ch == 'z') {
            if(g->players_mace > 0 && (g->players_weapon == -1 || g->players_weapon == 0)) {
                g->players_weapon = 0;
                choose = 0;
                break;
            }
            else if(g->players_mace == 0) {
                strcpy(message, "You have no mace                                       ");
            }
            else {
                strcpy(message, "You have to put your last weapon in the backpack first");
            }
        }
        else if (ch == 'x') {
            if(g->players_sword > 0 && (g->players_weapon == -1 || g->players_weapon == 1)) {
                g->players_weapon = 1;
                choose = 1;
                break;
            }
            else if(g->players_sword == 0) {
                strcpy(message, "You have no sword                                      ");
            }
            else {
                strcpy(message, "You have to put your last weapon in the backpack first");
            }
        }
        else if (ch == 'c') {
            if(g->players_dagger > 0 && (g->players_weapon == -1 || g->players_weapon == 2)) {
                g->players_weapon = 2;
                choose = 2;
                break;
            }
            else if(g->players_dagger == 0) {
                strcpy(message, "You have no dagger                                     ");
            }
            else {
                strcpy(message, "You have to put your last weapon in the backpack first");
            }
        }
        else if (ch == 'v') {
            if(g->players_magic_wand > 0 && (g->players_weapon == -1 || g->players_weapon == 3)) {
                g->players_weapon = 3;
                choose = 3;
                break;
            }
            else if(g->players_magic_wand == 0) {
                strcpy(message, "You have no magic wand                                 ");
            }
            else {
                strcpy(message, "You have to put your last weapon in the backpack first");
            }
        }
        else if (ch == 'b') {
            if(g->players_arrow > 0 && (g->players_weapon == -1 || g->players_weapon == 4)) {
                g->players_weapon = 4;
                choose = 4;
                break;
            }
            else if(g->players_arrow == 0) {
                strcpy(message, "You have no arrow                                      ");
            }
            else {
                strcpy(message, "You have to put your last weapon in the backpack first");
            }
        }
        else if (ch == ']') {
            if(g->players_broken_key >= 2) {
                g->players_ancient_key += 1;
                g->players_broken_key -= 2;
            }
            if(g->players_ancient_key > 0 && (g->players_weapon == -1 || g->players_weapon == 5)) {
                g->players_weapon = 5;
                choose = 5;
                break;
            }
            else if(g->players_ancient_key == 0) {
                strcpy(message, "You have no ancient key                                     ");
            }
            else {
                strcpy(message, "You have to put your other weapon in the backpack first");
            }
        }
        else {
            strcpy(message, "You pressed an invalid key                         ");
        }
    }
    clear();
}

void spell_screen(Game *g) {
    clear();

    draw_star();

    attron(COLOR_PAIR(5));
    mvprintw(5, 20, "SPELLS & EXILIRS");
    attroff(COLOR_PAIR(5));

    mvprintw(6, 20, "You can see spells you collected during the game here / Press enter to use spell");

    const char *spells[] = {"Health Potion", "Speed Potion", "Damage Potion", "RETURN"};

    int choose = 0;
    while (1) {
        for (int i=0; i<4; i++) {
            if (i == choose)
                attron(A_REVERSE);
            if(i == 0) {
                attron(COLOR_PAIR(3));
                mvprintw(12+i, 1, "%s (%d) ", spells[i], g->players_health_potion);
                attroff(COLOR_PAIR(3));
            }
            else if(i == 1) {
                attron(COLOR_PAIR(6));
                mvprintw(12+i, 1, "%s (%d) ", spells[i], g->players_speed_potion);
                attroff(COLOR_PAIR(6));
            }
            else if(i == 2) {
                attron(COLOR_PAIR(2));
                mvprintw(12+i, 1, "%s (%d) ", spells[i], g->players_damage_potion);
                attroff(COLOR_PAIR(2));
            }
            else {
                mvprintw(12+i, 1, "%s ", spells[i]);
            }
            if (i == choose)
                attroff(A_REVERSE);
        }

        int ch = getch();
        if (ch == KEY_UP && choose != 0)
            choose--;
        else if (ch == KEY_DOWN && choose != 3)
            choose++;
        else if (ch == 10) {
            if(choose == 0 && g->players_health_potion > 0 && g->players_health != 10) {
                g->players_health_step = g->players_steps;
                g->players_extra_health++;
                g->players_health_potion--;
                break;
            }
            else if(choose == 1 && g->players_speed_potion > 0) {
                g->players_speed_step = g->players_steps;
                g->players_speed = 2;
                g->players_speed_potion--;
                break;
            }
            else if(choose == 2 && g->players_damage_potion > 0) {
                g->players_damage_step = g->players_steps;
                g->players_damage_rate = 2;
                g->players_damage_potion--;
                break;
            }
            else if(choose == 3) {
                break;
            }
        }
    }
    clear();
}

void message_to_password(int password) {
    strcpy(message, "password: ");
    message[10] = (password/1000)+48; message[11] = (password/100 - (password/1000 * 10))+48; message[12] = ((password%100 - password%10)/10)+48; message[13] = (password%10)+48;
    message[14] = 0;
}

void password_screen(Game *g) {
    clear();
    echo();
    curs_set(TRUE);

    int try = 0;
    char password[5];
    int reverse = 0;
    if(rand()%5 == 3) {
        reverse = 1;
    }
    while(try<3) {
        mvprintw(LINES / 2, COLS / 2 - 12, "Enter the password: ");
        clrtobot();
        getnstr(password, 4);

        if(reverse) {
            char temp_password[5]; temp_password[0] = password[3]; temp_password[1] = password[2]; temp_password[2] = password[1]; temp_password[3] = password[0];
            strcpy(password, temp_password);
        }

        if(str_to_num(password) == g->rooms[g->k_lock].password) {
            g->rooms[g->k_lock].locked = 0;
            curs_set(FALSE);
            noecho();
            break;
        }
        else {
            try++;
            switch(try) {
                case 1:
                    attron(COLOR_PAIR(4));
                    mvprintw(LINES / 2 - 2, COLS / 2 - 12, "Your first attempt was false.   ");
                    attroff(COLOR_PAIR(4));
                    break;
                case 2:
                    attron(COLOR_PAIR(7));
                    mvprintw(LINES / 2 - 2, COLS / 2 - 12, "Your second attempt was false.  ");
                    attroff(COLOR_PAIR(7));
                    break;
                case 3:
                    attron(COLOR_PAIR(2));
                    mvprintw(LINES / 2 - 2, COLS / 2 - 12, "Your last try was INCORRECT!    ");
                    attroff(COLOR_PAIR(2));
                    curs_set(FALSE);
                    noecho();
                    int ch = getch();
                    break;
            }        
        }
    }        
}

void enchant_room(Player *p, Game *g) {
    clear();

    if(g->play_music) {play_music("creepypiano.mp3");}

    strcpy(message, "You've entered the ENCHANT ROOM!");

    int gold = 1 + rand()%2;
    int ordinary_food = 1 + rand()%2;
    int potion = 3 + rand()%4;

    int x_center = COLS/2;
    int y_center = LINES/2;

    Pos secret_door;
    
    int m=0;
    for(m; m<20; m++) {
        mvprintw(5+m, x_center-m-1, "/");
        for(int n=0; n<2*m+2; n++) {
            mvprintw(5+m, x_center-m+n, ".");
        }
        mvprintw(5+m, x_center+m+2, "\\");
    }
    for(int n=0; n<2*m+3; n++) {
        mvprintw(25, x_center-m+n, "-");
    }

    g->player_pos.x = x_center;
    g->player_pos.y = 5 + rand()%20;

    int count_gold = 0;
    int count_ordinary_food = 0;
    int count_potion = 0;

    m = 0;
    for(m; m<20; m++) {
        for(int n=0; n<2*m+2; n++) {
            if(rand()%20 == 0 && mvinch(5+m,x_center-m+n) == '.' && count_gold<gold) {
                mvprintw(5+m,x_center-m+n,"o");
                count_gold++;
            }
            if(rand()%20 == 0 && mvinch(5+m,x_center-m+n) == '.' && count_ordinary_food<ordinary_food) {
                mvprintw(5+m, x_center-m+n,"f");
                count_ordinary_food++;
            }
            if(rand()%20 == 0 && mvinch(5+m,x_center-m+n) == '.' && count_potion<potion) {
                int num = rand()%2 + rand()%2;
                if(num%3 == 0) {
                    attron(COLOR_PAIR(3));
                    mvprintw(5+m, x_center-m+n,"\"");
                    attroff(COLOR_PAIR(3));
                }
                else if(num%3 == 1) {
                    attron(COLOR_PAIR(6));
                    mvprintw(5+m, x_center-m+n,"*");
                    attroff(COLOR_PAIR(6));
                }
                else if(num%3 == 2) {
                    attron(COLOR_PAIR(2));
                    mvprintw(5+m, x_center-m+n,"'");
                    attroff(COLOR_PAIR(2));
                }
                count_potion++;
            }
        }
    }

    count_gold = 0; count_ordinary_food = 0; int count_secret_door = 0; count_potion = 0;
    for(m; m>=0; m--) {
        for(int n=0; n<2*m+2; n++) {
            if(rand()%20 == 0 && mvinch(5+m,x_center-m+n) == '.' && count_gold<gold) {
                mvprintw(5+m,x_center-m+n,"o");
                count_gold++;
            }
            if(rand()%20 == 0 && mvinch(5+m,x_center-m+n) == '.' && count_ordinary_food<ordinary_food) {
                mvprintw(5+m, x_center-m+n,"f");
                count_ordinary_food++;
            }
            if(rand()%20 == 0 && mvinch(5+m,x_center-m+n) == '.' && count_potion<potion) {
                int num = rand()%3;
                if(num%3 == 0) {
                    attron(COLOR_PAIR(3));
                    mvprintw(5+m, x_center-m+n,"\"");
                    attroff(COLOR_PAIR(3));
                }
                else if(num%3 == 1) {
                    attron(COLOR_PAIR(6));
                    mvprintw(5+m, x_center-m+n,"*");
                    attroff(COLOR_PAIR(6));
                }
                else if(num%3 == 2) {
                    attron(COLOR_PAIR(2));
                    mvprintw(5+m, x_center-m+n,"'");
                    attroff(COLOR_PAIR(2));
                }
                count_potion++;
            }
            if(rand()%20 == 0 && mvinch(5+m,x_center-m+n) == '.' && count_secret_door<1) {
                g->secret_doors_count = 1;
                g->secret_doors[0].x = x_center-m+n; g->secret_doors[0].y = 5+m;
                mvprintw(5+m, x_center-m+n,"?");
                count_secret_door++;
            }
        }
    }

    chtype **screen = (chtype **) malloc(COLS*sizeof(chtype*));
    for(int i=0; i<COLS; i++) {
        *(screen+i) = (chtype *) malloc(LINES*sizeof(chtype));
    }
    for(int j=1; j<LINES; j++) {
        for(int i=0; i<COLS; i++) {
            chtype character = mvinch(j,i);
            screen[i][j] = character;
        }
    }

    int **visited = (int **) malloc(COLS*sizeof(int *));
    for(int i=0; i<COLS; i++) {
        *(visited+i) = (int *) calloc(LINES,sizeof(int));
    }

    for(int j=1; j<LINES; j++) {
        for(int i=0; i<COLS; i++) {
            visited[i][j] = 1;
        }
    }

    int num = 0; int exit = 0;
    display_screen(g,"view",visited,screen);
    int health_loss = 2;
    while(1) {
        display_message(g, g->floor_number, g->players_score, g->players_gold);
        display_health(g);
        time_t current_time = time(NULL);
        double elapsed_time = difftime(current_time, g->enchant_start_time);

        if(elapsed_time >= 4) {
            g->enchant_start_time = time(NULL);
            num++;
        }
 
        if(num == 4) {
            g->players_health -= health_loss;
            health_loss ++;
            num = 0;
        }

        if(g->players_steps - g->players_speed_step >= 15) {g->players_speed = 1; g->players_speed_step = -15;}
        if(g->players_steps - g->players_health_step >= 30) {g->players_extra_health = 0; g->players_health_step = -30;}
        if(g->players_steps - g->players_damage_step >= 10) {g->players_damage_step = -10;}

        if(g->players_health + g->players_extra_health <= 0) {
            terminate_game(0, p, g);
        }

        display_screen(g,"view",visited,screen);
        draw_spiral(num);

        attron(COLOR_PAIR(g->player_color));
        mvprintw(g->player_pos.y, g->player_pos.x, "@");
        attroff(COLOR_PAIR(g->player_color));

        int ch = getch();

        if(ch == 'e') {
            food_screen(g);
        }
        else if(ch == 'i') {
            weapon_screen(g);
        }
        else if(ch == 'p') {
            spell_screen(g);
        }
        switch(handle_movement(screen, visited, ch, g)) {
            case 5:
                exit = 1;
                break;
        }
        if(exit) {
            stop_music();
            break;
        }
    }
}

void play_music(const char *filename) {
    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1) {
        fprintf(stderr, "Mix_OpenAudio: %s", Mix_GetError());
        exit(1);
    }

    Mix_Music *music = Mix_LoadMUS(filename);
    if (!music) {
        fprintf(stderr, "Mix_LoadMUS: %s", Mix_GetError());
        exit(1);
    }

    Mix_PlayMusic(music, -1);
}

void stop_music() {
    Mix_HaltMusic();
    Mix_CloseAudio();
}

void handle_monsters(Game *g, chtype **screen) {
    for(int k=0; k<6; k++) {
        for(int m=0; m<g->rooms[k].monsters_count; m++) {
            if(g->rooms[k].monsters[m].alive == 0) {
                g->rooms[k].monsters[m].on  = 0;
            }
            if(k != check_room(g->rooms, g->player_pos.x, g->player_pos.y)) {
                g->rooms[k].monsters[m].on  = 0;
            }
            else {
                if(abs(g->player_pos.x - g->rooms[k].monsters[m].position.x) <= g->rooms[k].monsters[m].radius && abs(g->player_pos.y - g->rooms[k].monsters[m].position.y) <= g->rooms[k].monsters[m].radius) {
                    g->rooms[k].monsters[m].on  = 1;
                }
            }
        }
    }
}

void handle_movement_monsters(Game *g, chtype **screen) {
    for(int k=0; k<6; k++) {
        for(int m=0; m<g->rooms[k].monsters_count; m++) {
            int delta_x;
            int delta_y;
            if(g->rooms[k].monsters[m].alive && g->rooms[k].monsters[m].on && !g->rooms[k].monsters[m].freeze) {
                if(g->rooms[k].monsters[m].haunt < 0) {
                    if(g->rooms[k].monsters[m].type == 1) {
                        g->rooms[k].monsters[m].on = 0;
                        g->rooms[k].monsters[m].haunt = 1;
                    }
                    else if(g->rooms[k].monsters[m].type == 2) {
                        if(abs(g->rooms[k].monsters[m].position.x - g->player_pos.x) > g->rooms[k].monsters[m].radius || abs(g->rooms[k].monsters[m].position.y - g->player_pos.y) > g->rooms[k].monsters[m].radius) {
                            g->rooms[k].monsters[m].on = 0;
                            g->rooms[k].monsters[m].haunt = 3;
                        }
                    }
                    else {
                        if(g->rooms[k].monsters[m].position.x > g->player_pos.x) {
                            delta_x = 1;
                        }
                        else if(g->rooms[k].monsters[m].position.x == g->player_pos.x) {
                            delta_x = 0;
                        }
                        else {
                            delta_x = -1;
                        }
                        if(g->rooms[k].monsters[m].position.y > g->player_pos.y) {
                            delta_y = 1;
                        }
                        else if(g->rooms[k].monsters[m].position.y == g->player_pos.y) {
                            delta_y = 0;
                        }
                        else {
                            delta_y = -1;
                        }
                        char ch = screen[g->rooms[k].monsters[m].position.x + delta_x][g->rooms[k].monsters[m].position.y + delta_y] & A_CHARTEXT;
                        if(ch == '.') {
                            g->rooms[k].monsters[m].position.x += delta_x; g->rooms[k].monsters[m].position.y += delta_y;
                        }

                        if(abs(g->rooms[k].monsters[m].position.x - g->player_pos.x) > g->rooms[k].monsters[m].radius+1 || abs(g->rooms[k].monsters[m].position.y - g->player_pos.y) > g->rooms[k].monsters[m].radius+1) {
                            if(g->rooms[k].monsters[m].type == 3) {g->rooms[k].monsters[m].on = 0; g->rooms[k].monsters[m].haunt = 5;}
                            else if(g->rooms[k].monsters[m].type == 5) {g->rooms[k].monsters[m].on = 0; g->rooms[k].monsters[m].haunt = 8;}
                        }
                    }
                    
                    return;
                }

                if(is_next(g->rooms[k].monsters[m].position, g->player_pos)) {

                    if(g->players_extra_health > 0 && (g->players_extra_health - g->rooms[k].monsters[m].damage) > 0) {
                        g->players_extra_health -= g->rooms[k].monsters[m].damage;
                    }
                    else {
                        g->players_health -= g->rooms[k].monsters[m].damage;
                    }
                    if(g->rooms[k].monsters[m].type == 1) {strcpy(message, "Daemon scored an excellent hit on you!");}
                    else if(g->rooms[k].monsters[m].type == 2) {strcpy(message, "Fire Breathing Monster scored an excellent hit on you!");}
                    else if(g->rooms[k].monsters[m].type == 3) {strcpy(message, "Giant scored an excellent hit on you!");}
                    else if(g->rooms[k].monsters[m].type == 4) {strcpy(message, "Snake scored an excellent hit on you!");}
                    else if(g->rooms[k].monsters[m].type == 5) {strcpy(message, "Undead scored an excellent hit on you!");}
                }
                
                if(g->rooms[k].monsters[m].type == 1) {
                    g->rooms[k].monsters[m].haunt -= 1;
                }
                else {
                    if(g->player_pos.x > g->rooms[k].monsters[m].position.x) {
                        delta_x = 1;
                    }
                    else if(g->player_pos.x == g->rooms[k].monsters[m].position.x) {
                        delta_x = 0;
                    }
                    else {
                        delta_x = -1;
                    }
                    if(g->player_pos.y > g->rooms[k].monsters[m].position.y) {
                        delta_y = 1;
                    }
                    else if(g->player_pos.y == g->rooms[k].monsters[m].position.y) {
                        delta_y = 0;
                    }
                    else {
                        delta_y = -1;
                    }
                    char ch = screen[g->rooms[k].monsters[m].position.x + delta_x][g->rooms[k].monsters[m].position.y + delta_y] & A_CHARTEXT;
                    if((ch == '.' && (g->rooms[k].monsters[m].position.x + delta_x != g->player_pos.x || g->rooms[k].monsters[m].position.y + delta_y != g->player_pos.y)) ||  (g->rooms[k].monsters[m].type == 4 && (ch == '.' || ch == '+' || ch == '#') && (g->rooms[k].monsters[m].position.x + delta_x != g->player_pos.x || g->rooms[k].monsters[m].position.y + delta_y != g->player_pos.y))) {
                        g->rooms[k].monsters[m].position.x += delta_x; g->rooms[k].monsters[m].position.y += delta_y;
                        g->rooms[k].monsters[m].haunt -= 1;
                    }
                }
            } 
        }
    }
}

int is_next(Pos a, Pos b) {
    if((abs(a.x - b.x) <= 1) && (abs(a.y - b.y) <= 1)) {
        return 1;
    }
    return 0;
}

void handle_weapons(Game *g, char ch, chtype **screen) {
    switch(g->players_weapon) {
        case 2:
            if(g->players_dagger == 0) {
                g->players_weapon = -1;
            }
            break;
        case 3:
            if(g->players_magic_wand == 0) {
                g->players_weapon = -1;
            }
            break;
        case 4:
            if(g->players_arrow == 0) {
                g->players_weapon = -1;
            }
            break;
        case 5:
            if(g->players_ancient_key == 0) {
                g->players_weapon = -1;
            }
            break;
    }

    if (ch == 'w') {
        g->players_weapon = -1;
    }
    else if (ch == 'z') {
        if(g->players_mace > 0 && (g->players_weapon == -1 || g->players_weapon == 0)) {
            g->players_weapon = 0;
            strcpy(message, "weapon changed to mace");
        }
        else if(g->players_mace == 0) {
            strcpy(message, "You have no mace");
        }
        else {
            strcpy(message, "put in the backpack first");
        }
    }
    else if (ch == 'x') {
        if(g->players_sword > 0 && (g->players_weapon == -1 || g->players_weapon == 1)) {
            g->players_weapon = 1;
            strcpy(message, "weapon changed to sword");
        }
        else if(g->players_sword == 0) {
            strcpy(message, "You have no sword");
        }
        else {
            strcpy(message, "put in the backpack first");
        }
    }
    else if (ch == 'c') {
        if(g->players_dagger > 0 && (g->players_weapon == -1 || g->players_weapon == 2)) {
            g->players_weapon = 2;
            strcpy(message, "weapon changed to dagger");
        }
        else if(g->players_dagger == 0) {
            strcpy(message, "You have no dagger");
        }
        else {
            strcpy(message, "put in the backpack first");
        }
    }
    else if (ch == 'v') {
        if(g->players_magic_wand > 0 && (g->players_weapon == -1 || g->players_weapon == 3)) {
            g->players_weapon = 3;
            strcpy(message, "weapon changed to magic wand");
        }
        else if(g->players_magic_wand == 0) {
            strcpy(message, "You have no magic wand");
        }
        else {
            strcpy(message, "put in the backpack first");
        }
    }
    else if (ch == 'b') {
        if(g->players_arrow > 0 && (g->players_weapon == -1 || g->players_weapon == 4)) {
            g->players_weapon = 4;
            strcpy(message, "weapon changed to arrow");
        }
        else if(g->players_arrow == 0) {
            strcpy(message, "You have no arrow");
        }
        else {
            strcpy(message, "put in the backpack first");
        }
    }
    else if (ch == ']') {
        if(g->players_ancient_key > 0 && (g->players_weapon == -1 || g->players_weapon == 5)) {
            g->players_weapon = 5;
            strcpy(message, "You can now open any locked doors!");
        }
        else if(g->players_ancient_key == 0) {
            strcpy(message, "You have no ancient key");
        }
        else {
            strcpy(message, "put your other weapon in the backpack first");
        }
    }
    if (ch == ' ') {
        if(g->players_weapon == 0 || g->players_weapon == 1) {
            for(int i=g->player_pos.x-1; i<=g->player_pos.x+1; i++) {
                for(int j=g->player_pos.y-1; j<=g->player_pos.y+1; j++) {
                    int k = check_room(g->rooms, i, j);
                    for(int m=0; m<g->rooms[k].monsters_count; m++) {
                        if(g->rooms[k].monsters[m].position.x == i && g->rooms[k].monsters[m].position.y == j && g->rooms[k].monsters[m].alive) {
                            if(g->players_weapon == 0) {g->rooms[k].monsters[m].health -= g->players_damage_rate*5;}
                            else {g->rooms[k].monsters[m].health -= g->players_damage_rate*10;}
                            int type = check_monsters(g, i, j);
                            if(type == 1) {strcpy(message, "You scored an excellent hit on Daemon"); g->players_score += 5;}
                            else if(type == 2) {strcpy(message, "You scored an excellent hit on Fire Breathing Monster"); g->players_score += 5;}
                            else if(type == 3) {strcpy(message, "You scored an excellent hit on Giant"); g->players_score += 5;}
                            else if(type == 4) {strcpy(message, "You scored an excellent hit on Snake"); g->players_score += 10;}
                            else if(type == 5) {strcpy(message, "You scored an excellent hit on Undead"); g->players_score += 10;}
                            if(g->rooms[k].monsters[m].health <= 0) {
                                g->rooms[k].monsters[m].alive = 0;
                                if(g->rooms[k].monsters[m].type == 1) {strcpy(message, "You killed the Daemon!"); g->players_score += 10;}
                                else if(g->rooms[k].monsters[m].type == 2) {strcpy(message, "You killed the Fire Breathing Monster!"); g->players_score += 10;}
                                else if(g->rooms[k].monsters[m].type == 3) {strcpy(message, "You killed the Giant!"); g->players_score += 10;}
                                else if(g->rooms[k].monsters[m].type == 4) {strcpy(message, "You killed the Snake!"); g->players_score += 20;}
                                else if(g->rooms[k].monsters[m].type == 5) {strcpy(message, "You killed the Undead!"); g->players_score += 20;}
                            }
                        }
                    }
                }
            }
        }
        else {
            ch = getch();
            g->players_weapon_direction = ch;
            shoot_weapon(g, ch, screen);
        }
    }
    if (ch == 'a') {
        shoot_weapon(g, g->players_weapon_direction, screen);
    }
    else {
        return;
    }
}

void shoot_weapon(Game *g, char ch, chtype **screen) {
    int range;
    if(g->players_weapon == 2) {range = 5;}
    else if(g->players_weapon == 3) {range = 10;}
    else if(g->players_weapon == 4) {range = 5;}
    if(ch == '6') {
        for(int i=g->player_pos.x; i<=g->player_pos.x+range; i++) {
            int j=g->player_pos.y;
            char character = screen[i][j] & A_CHARTEXT;
            if(character == '|' || character == 'O' || character == '+') {if(g->players_weapon == 2) {g->players_dagger -= 1; screen[i-1][j] = '~';} else if(g->players_weapon == 3) {g->players_magic_wand -= 1; screen[i-1][j] = ('!' | COLOR_PAIR(5));} else if(g->players_weapon == 4) {g->players_arrow -= 1; screen[i-1][j] = '}';} break;}
            int k = check_room(g->rooms, i, j);
            int shot = 0;
            for(int m=0; m<g->rooms[k].monsters_count; m++) {
                if(g->rooms[k].monsters[m].position.x == i && g->rooms[k].monsters[m].position.y == j && g->rooms[k].monsters[m].alive) {
                    if(g->players_weapon == 2) {g->players_dagger -= 1; g->rooms[k].monsters[m].health -= g->players_damage_rate*12;}
                    else if(g->players_weapon == 3) {g->players_magic_wand -= 1; g->rooms[k].monsters[m].health -= g->players_damage_rate*15; g->rooms[k].monsters[m].freeze = 1;}
                    else if(g->players_weapon == 4) {g->players_arrow -= 1; g->rooms[k].monsters[m].health -= g->players_damage_rate*5;}
                    int type = check_monsters(g, i, j);
                    if(type == 1) {strcpy(message, "You scored an excellent hit on Daemon"); g->players_score += 5;}
                    else if(type == 2) {strcpy(message, "You scored an excellent hit on Fire Breathing Monster"); g->players_score += 5;}
                    else if(type == 3) {strcpy(message, "You scored an excellent hit on Giant"); g->players_score += 5;}
                    else if(type == 4) {strcpy(message, "You scored an excellent hit on Snake"); g->players_score += 10;}
                    else if(type == 5) {strcpy(message, "You scored an excellent hit on Undead"); g->players_score += 10;}
                    if(g->rooms[k].monsters[m].health <= 0) {
                        g->rooms[k].monsters[m].alive = 0;
                        if(g->rooms[k].monsters[m].type == 1) {strcpy(message, "You killed the Daemon!"); g->players_score += 10;}
                        else if(g->rooms[k].monsters[m].type == 2) {strcpy(message, "You killed the Fire Breathing Monster!"); g->players_score += 10;}
                        else if(g->rooms[k].monsters[m].type == 3) {strcpy(message, "You killed the Giant!"); g->players_score += 10;}
                        else if(g->rooms[k].monsters[m].type == 4) {strcpy(message, "You killed the Snake!"); g->players_score += 20;}
                        else if(g->rooms[k].monsters[m].type == 5) {strcpy(message, "You killed the Undead!"); g->players_score += 20;}
                    }
                    shot = 1;
                }
            }
            if(shot) {break;}
            if(i == g->player_pos.x+range) {if(g->players_weapon == 2) {g->players_dagger -= 1; screen[i][j] = '~';} else if(g->players_weapon == 3) {g->players_magic_wand -= 1; screen[i][j] = ('!' | COLOR_PAIR(5));} else if(g->players_weapon == 4) {g->players_arrow -= 1; screen[i][j] = '}';}}
        }
    }
    else if(ch == '4') {
        for(int i=g->player_pos.x; i>=g->player_pos.x-range; i--) {
            int j=g->player_pos.y;
            char character = screen[i][j] & A_CHARTEXT;
            if(character == '|' || character == 'O' || character == '+') {if(g->players_weapon == 2) {g->players_dagger -= 1; screen[i+1][j] = '~';} else if(g->players_weapon == 3) {g->players_magic_wand -= 1; screen[i+1][j] = ('!' | COLOR_PAIR(5));} else if(g->players_weapon == 4) {g->players_arrow -= 1; screen[i+1][j] = '}';} break;}
            int k = check_room(g->rooms, i, j);
            int shot = 0;
            for(int m=0; m<g->rooms[k].monsters_count; m++) {
                if(g->rooms[k].monsters[m].position.x == i && g->rooms[k].monsters[m].position.y == j && g->rooms[k].monsters[m].alive) {
                    if(g->players_weapon == 2) {g->players_dagger -= 1; g->rooms[k].monsters[m].health -= g->players_damage_rate*12;}
                    else if(g->players_weapon == 3) {g->players_magic_wand -= 1; g->rooms[k].monsters[m].health -= g->players_damage_rate*15; g->rooms[k].monsters[m].freeze = 1;}
                    else if(g->players_weapon == 4) {g->players_arrow -= 1; g->rooms[k].monsters[m].health -= g->players_damage_rate*5;}
                    int type = check_monsters(g, i, j);
                    if(type == 1) {strcpy(message, "You scored an excellent hit on Daemon"); g->players_score += 5;}
                    else if(type == 2) {strcpy(message, "You scored an excellent hit on Fire Breathing Monster"); g->players_score += 5;}
                    else if(type == 3) {strcpy(message, "You scored an excellent hit on Giant"); g->players_score += 5;}
                    else if(type == 4) {strcpy(message, "You scored an excellent hit on Snake"); g->players_score += 10;}
                    else if(type == 5) {strcpy(message, "You scored an excellent hit on Undead"); g->players_score += 10;}
                    if(g->rooms[k].monsters[m].health <= 0) {
                        g->rooms[k].monsters[m].alive = 0;
                        if(g->rooms[k].monsters[m].type == 1) {strcpy(message, "You killed the Daemon!"); g->players_score += 10;}
                        else if(g->rooms[k].monsters[m].type == 2) {strcpy(message, "You killed the Fire Breathing Monster!"); g->players_score += 10;}
                        else if(g->rooms[k].monsters[m].type == 3) {strcpy(message, "You killed the Giant!"); g->players_score += 10;}
                        else if(g->rooms[k].monsters[m].type == 4) {strcpy(message, "You killed the Snake!"); g->players_score += 20;}
                        else if(g->rooms[k].monsters[m].type == 5) {strcpy(message, "You killed the Undead!"); g->players_score += 20;}
                    }
                    shot = 1;
                }
            }
            if(shot) {break;}
            if(i == g->player_pos.x-range) {if(g->players_weapon == 2) {g->players_dagger -= 1; screen[i][j] = '~';} else if(g->players_weapon == 3) {g->players_magic_wand -= 1; screen[i][j] = ('!' | COLOR_PAIR(5));} else if(g->players_weapon == 4) {g->players_arrow -= 1; screen[i][j] = '}';}}
        }
    }
    else if(ch == '2') {
        for(int j=g->player_pos.y; j<=g->player_pos.y+range; j++) {
            int i=g->player_pos.x;
            char character = screen[i][j] & A_CHARTEXT;
            if(character == '-' || character == 'O' || character == '+') {if(g->players_weapon == 2) {g->players_dagger -= 1; screen[i][j-1] = '~';} else if(g->players_weapon == 3) {g->players_magic_wand -= 1; screen[i][j-1] = ('!' | COLOR_PAIR(5));} else if(g->players_weapon == 4) {g->players_arrow -= 1; screen[i][j-1] = '}';} break;}
            int k = check_room(g->rooms, i, j);
            int shot = 0;
            for(int m=0; m<g->rooms[k].monsters_count; m++) {
                if(g->rooms[k].monsters[m].position.x == i && g->rooms[k].monsters[m].position.y == j && g->rooms[k].monsters[m].alive) {
                    if(g->players_weapon == 2) {g->players_dagger -= 1; g->rooms[k].monsters[m].health -= g->players_damage_rate*12;}
                    else if(g->players_weapon == 3) {g->players_magic_wand -= 1; g->rooms[k].monsters[m].health -= g->players_damage_rate*15; g->rooms[k].monsters[m].freeze = 1;}
                    else if(g->players_weapon == 4) {g->players_arrow -= 1; g->rooms[k].monsters[m].health -= g->players_damage_rate*5;}
                    int type = check_monsters(g, i, j);
                    if(type == 1) {strcpy(message, "You scored an excellent hit on Daemon"); g->players_score += 5;}
                    else if(type == 2) {strcpy(message, "You scored an excellent hit on Fire Breathing Monster"); g->players_score += 5;}
                    else if(type == 3) {strcpy(message, "You scored an excellent hit on Giant"); g->players_score += 5;}
                    else if(type == 4) {strcpy(message, "You scored an excellent hit on Snake"); g->players_score += 10;}
                    else if(type == 5) {strcpy(message, "You scored an excellent hit on Undead"); g->players_score += 10;}
                    if(g->rooms[k].monsters[m].health <= 0) {
                        g->rooms[k].monsters[m].alive = 0;
                        if(g->rooms[k].monsters[m].type == 1) {strcpy(message, "You killed the Daemon!"); g->players_score += 10;}
                        else if(g->rooms[k].monsters[m].type == 2) {strcpy(message, "You killed the Fire Breathing Monster!"); g->players_score += 10;}
                        else if(g->rooms[k].monsters[m].type == 3) {strcpy(message, "You killed the Giant!"); g->players_score += 10;}
                        else if(g->rooms[k].monsters[m].type == 4) {strcpy(message, "You killed the Snake!"); g->players_score += 20;}
                        else if(g->rooms[k].monsters[m].type == 5) {strcpy(message, "You killed the Undead!"); g->players_score += 20;}
                    }
                    shot = 1;
                }
            }
            if(shot) {break;}
            if(j == g->player_pos.y+range) {if(g->players_weapon == 2) {g->players_dagger -= 1; screen[i][j] = '~';} else if(g->players_weapon == 3) {g->players_magic_wand -= 1; screen[i][j] = ('!' | COLOR_PAIR(5));} else if(g->players_weapon == 4) {g->players_arrow -= 1; screen[i][j] = '}';}}
        }
    }
    else if(ch == '8') {
        for(int j=g->player_pos.y; j>=g->player_pos.y-range; j--) {
            int i=g->player_pos.x;
            char character = screen[i][j] & A_CHARTEXT;
            if(character == '-' || character == 'O' || character == '+') {if(g->players_weapon == 2) {g->players_dagger -= 1; screen[i][j+1] = '~';} else if(g->players_weapon == 3) {g->players_magic_wand -= 1; screen[i][j+1] = ('!' | COLOR_PAIR(5));} else if(g->players_weapon == 4) {g->players_arrow -= 1; screen[i][j+1] = '}';} break;}
            int k = check_room(g->rooms, i, j);
            int shot = 0;
            for(int m=0; m<g->rooms[k].monsters_count; m++) {
                if(g->rooms[k].monsters[m].position.x == i && g->rooms[k].monsters[m].position.y == j && g->rooms[k].monsters[m].alive) {
                    if(g->players_weapon == 2) {g->players_dagger -= 1; g->rooms[k].monsters[m].health -= g->players_damage_rate*12;}
                    else if(g->players_weapon == 3) {g->players_magic_wand -= 1; g->rooms[k].monsters[m].health -= g->players_damage_rate*15; g->rooms[k].monsters[m].freeze = 1;}
                    else if(g->players_weapon == 4) {g->players_arrow -= 1; g->rooms[k].monsters[m].health -= g->players_damage_rate*5;}
                    int type = check_monsters(g, i, j);
                    if(type == 1) {strcpy(message, "You scored an excellent hit on Daemon"); g->players_score += 5;}
                    else if(type == 2) {strcpy(message, "You scored an excellent hit on Fire Breathing Monster"); g->players_score += 5;}
                    else if(type == 3) {strcpy(message, "You scored an excellent hit on Giant"); g->players_score += 5;}
                    else if(type == 4) {strcpy(message, "You scored an excellent hit on Snake"); g->players_score += 10;}
                    else if(type == 5) {strcpy(message, "You scored an excellent hit on Undead"); g->players_score += 10;}
                    if(g->rooms[k].monsters[m].health <= 0) {
                        g->rooms[k].monsters[m].alive = 0;
                        if(g->rooms[k].monsters[m].type == 1) {strcpy(message, "You killed the Daemon!"); g->players_score += 10;}
                        else if(g->rooms[k].monsters[m].type == 2) {strcpy(message, "You killed the Fire Breathing Monster!"); g->players_score += 10;}
                        else if(g->rooms[k].monsters[m].type == 3) {strcpy(message, "You killed the Giant!"); g->players_score += 10;}
                        else if(g->rooms[k].monsters[m].type == 4) {strcpy(message, "You killed the Snake!"); g->players_score += 20;}
                        else if(g->rooms[k].monsters[m].type == 5) {strcpy(message, "You killed the Undead!"); g->players_score += 20;}
                    }
                    shot = 1;
                }
            }
            if(shot) {break;}
            if(j == g->player_pos.y-range) {if(g->players_weapon == 2) {g->players_dagger -= 1; screen[i][j] = '~';} else if(g->players_weapon == 3) {g->players_magic_wand -= 1; screen[i][j] = ('!' | COLOR_PAIR(5));} else if(g->players_weapon == 4) {g->players_arrow -= 1; screen[i][j] = '}';}}
        }
    }
    else {
        return;
    }
}

int check_weapons(Pos *weapons, int i, int j) {
    for(int w=0; w<3; w++) {
        if(weapons[w].x == i && weapons[w].y == j) {
            return 1;
        }
    }
    return 0;
}

void battle_room(char mode[], Player *p, Game *g) {
    srand(time(NULL));
    clear();

    int gold;
    int food;
    int potion;
    if(strcmp(mode, "trap") == 0) {
        strcpy(message, "You're now in the battle room!");
        g->rooms[0].monsters_count = 4 + rand()%2;
        gold = 3 + rand()%4;
        food = 3 + rand()%2;
        if(g->play_music) {play_music("thefinaldream.mp3");}
    }
    else if(strcmp(mode, "treasure") == 0) {
        strcpy(message, "You're now in the treasure room!");
        g->rooms[0].type = 1;
        g->rooms[0].monsters_count = 6 + rand()%3;
        gold = 6 + rand()%5;
        food = 4 + rand()%3;
        if(g->play_music) {play_music("ComePlay.mp3");}
    }
    
    for(int i=0; i<g->rooms[0].monsters_count; i++) {
        if(strcmp(mode, "trap") == 0) {g->rooms[0].monsters[i].type = 1+rand()%3;} else if(strcmp(mode, "treasure") == 0) {g->rooms[0].monsters[i].type = 1+rand()%5;}
        g->rooms[0].monsters[i].alive = 1; g->rooms[0].monsters[i].on = 0; g->rooms[0].monsters[i].freeze = 0; g->rooms[0].monsters[i].room = 0;
        if(g->rooms[0].monsters[i].type == 1) {g->rooms[0].monsters[i].radius = 1; g->rooms[0].monsters[i].health = 5; g->rooms[0].monsters[i].damage = 1; g->rooms[0].monsters[i].haunt = 0;}
        else if(g->rooms[0].monsters[i].type == 2) {g->rooms[0].monsters[i].radius = 3; g->rooms[0].monsters[i].health = 10; g->rooms[0].monsters[i].damage = 1; g->rooms[0].monsters[i].haunt = 3;}
        else if(g->rooms[0].monsters[i].type == 3) {g->rooms[0].monsters[i].radius = 4; g->rooms[0].monsters[i].health = 15; g->rooms[0].monsters[i].damage = 1; g->rooms[0].monsters[i].haunt = 5;}
        else if(g->rooms[0].monsters[i].type == 4) {g->rooms[0].monsters[i].radius = 5; g->rooms[0].monsters[i].health = 20; g->rooms[0].monsters[i].damage = 1; g->rooms[0].monsters[i].haunt = 100;}
        else if(g->rooms[0].monsters[i].type == 5) {g->rooms[0].monsters[i].radius = 10; g->rooms[0].monsters[i].health = 30; g->rooms[0].monsters[i].damage = 2; g->rooms[0].monsters[i].haunt = 8;}
    }

    potion = 3 + rand()%2;

    for(int j=LINES/2-5; j<=LINES/2+12; j++) {
        for(int i=COLS/2-30; i<=COLS/2+30; i++) {
            if((j == LINES/2-5 || j == LINES/2+12) && abs(i-COLS/2) <= 30) {
                if(strcmp(mode, "trap") == 0) {attron(COLOR_PAIR(4));} else if(strcmp(mode, "treasure") == 0) {attron(COLOR_PAIR(5));}
                mvprintw(j, i, "-");
                if(strcmp(mode, "trap") == 0) {attroff(COLOR_PAIR(4));} else if(strcmp(mode, "treasure") == 0) {attroff(COLOR_PAIR(5));}
            }
            else if((j > LINES/2-5 && j < LINES/2+12) && abs(i-COLS/2) == 30) {
                if(strcmp(mode, "trap") == 0) {attron(COLOR_PAIR(4));} else if(strcmp(mode, "treasure") == 0) {attron(COLOR_PAIR(5));}
                mvprintw(j, i, "|");
                if(strcmp(mode, "trap") == 0) {attroff(COLOR_PAIR(4));} else if(strcmp(mode, "treasure") == 0) {attroff(COLOR_PAIR(5));}
            }
            else if((j > LINES/2-5 && j < LINES/2+12) && abs(i-COLS/2) < 30) {
                if(strcmp(mode, "treasure") == 0) {attron(COLOR_PAIR(4));}
                mvprintw(j, i, ".");
                if(strcmp(mode, "treasure") == 0) {attroff(COLOR_PAIR(4));}
            }
        }
    }

    if(strcmp(mode, "trap") == 0) {draw_big_trophy(4);} else if(strcmp(mode, "treasure") == 0) {draw_big_trophy(5);}
    
    int gold_count = 0; int food_count = 0; int potion_count = 0;
    int monster = 0;
    for(int j=LINES/2-5+1; j<LINES/2+12; j++) {
        for(int i=COLS/2-30+1; i<COLS/2+30; i++) {

            if(rand()%((g->rooms[2].room_size_h*g->rooms[2].room_size_v)) == 0 && (mvinch(j,i) & A_CHARTEXT) == '.' && gold_count<gold) {
                mvprintw(j,i,"o");
                gold_count++;
            }
            
            if(rand()%((g->rooms[3].room_size_h*g->rooms[3].room_size_v)) == 0 && (mvinch(j,i) & A_CHARTEXT) == '.' && food_count<food) {
                mvprintw(j,i,"f");
                food_count++;
            }
            if(rand()%((g->rooms[4].room_size_h*g->rooms[4].room_size_v)) == 0 && (mvinch(j,i) & A_CHARTEXT) == '.' && monster<g->rooms[0].monsters_count) {
                g->rooms[0].monsters[monster].position.x = i; g->rooms[0].monsters[monster].position.y = j;
                monster++;
            }
            if(rand()%((g->rooms[5].room_size_h*g->rooms[5].room_size_v)) == 0 && (mvinch(j,i) & A_CHARTEXT) == '.' && potion_count<potion) {
                int num = rand()%3;
                if(num%3 == 0) {
                    attron(COLOR_PAIR(3));
                    mvprintw(j, i,"\"");
                    attroff(COLOR_PAIR(3));
                }
                else if(num%3 == 1) {
                    attron(COLOR_PAIR(6));
                    mvprintw(j, i,"*");
                    attroff(COLOR_PAIR(6));
                }
                else if(num%3 == 2) {
                    attron(COLOR_PAIR(2));
                    mvprintw(j, i,"'");
                    attroff(COLOR_PAIR(2));
                }
                potion_count++;
            }
            
        }
    }

    chtype **screen = (chtype **) malloc(COLS*sizeof(chtype*));
    for(int i=0; i<COLS; i++) {
        *(screen+i) = (chtype *) malloc(LINES*sizeof(chtype));
    }
    for(int j=1; j<LINES; j++) {
        for(int i=0; i<COLS; i++) {
            chtype character = mvinch(j,i);
            screen[i][j] = character;
        }
    }
    clear();

    int **visited = (int **) malloc(COLS*sizeof(int *));
    for(int i=0; i<COLS; i++) {
        *(visited+i) = (int *) calloc(LINES,sizeof(int));
    }

    for(int j=1; j<LINES; j++) {
        for(int i=0; i<COLS; i++) {
            visited[i][j] = 1;
        }
    }

    g->player_pos.x = COLS/2-30+1+rand()%59; g->player_pos.y = LINES/2+rand()%10;
    display_screen(g,"view",visited,screen);
    while(1) {
        display_message(g, g->floor_number, g->players_score, g->players_gold);
        display_health(g);
        time_t current_time = time(NULL);
        double elapsed_time = difftime(current_time, g->start_time);

        if(elapsed_time > 30) {
            if(g->players_hungriness == 0 && g->players_health != 10) {
                g->players_health += 1;
            }
            if(g->players_hungriness < 10) {
                g->players_hungriness += g->hungriness_rate;
            }
            g->start_time = time(NULL);
            if(g->players_hungriness >= 5) {
                g->players_health -= g->players_hungriness/5;
            }
        }

        if(g->players_steps - g->players_speed_step >= 15) {g->players_speed = 1; g->players_speed_step = -15;}
        if(g->players_steps - g->players_health_step >= 30) {g->players_extra_health = 0; g->players_health_step = -30;}
        if(g->players_steps - g->players_damage_step >= 10) {g->players_damage_rate = 1; g->players_damage_step = -10;}

        if(g->players_health + g->players_extra_health <= 0) {
            terminate_game(0, p, g);
        }

        display_screen(g,"view",visited,screen);

        attron(COLOR_PAIR(g->player_color));
        mvprintw(g->player_pos.y, g->player_pos.x, "@");
        attroff(COLOR_PAIR(g->player_color));

        int ch = getch();
        if(ch == 'e') {
            food_screen(g);
        }
        else if(ch == 'i') {
            weapon_screen(g);
        }
        else if(ch == 'p') {
            spell_screen(g);
        }

        handle_monsters(g, screen);
        handle_movement_monsters(g, screen);

        handle_weapons(g, ch, screen);

        display_health(g);

        handle_movement(screen, visited, ch, g);

        int check = 0;
        for(int i=0; i<g->rooms[0].monsters_count; i++) {
            check = check || g->rooms[0].monsters[i].alive;
        }
        if(!check) {
            if(strcmp(mode, "trap") == 0) {
                stop_music();
                break;
            }
            else if(strcmp(mode, "treasure") == 0) {
                stop_music();
                terminate_game(1, p, g);
            }
        }
    }
}