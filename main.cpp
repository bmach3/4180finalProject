//=================================================================
// The main program file.
//
// Copyright 2021 Georgia Tech.  All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must not be made publicly available anywhere.
//==================================================================

// External libs
#include <stdlib.h>
#include <math.h>
// Project includes
#include "globals.h"
#include "hardware.h"
#include "city_landscape_public.h"
#include "missile_public.h"
#include "player_public.h"

#define CITY_HIT_MARGIN 1
#define CITY_UPPER_BOUND (SIZE_Y-(LANDSCAPE_HEIGHT+MAX_BUILDING_HEIGHT))
char buffer[20];
int bullets = 30;
int level = 1;
int num_city_g = 4;
int cities = num_city_g;
int missilescore;
int speed = 6;
int interval = 10;
int currenthighscore;
// function prototypes

void set_random_seed(Timer);
int city_landscape_update(void);
int was_player_hit(void);
void missile_contact(void); 

int main()
{
    mkdir("/sd/dir", 0777); // init directory
    GameInputs inputs; 
    // First things first: initialize hardware
    ASSERT_P(hardware_init() == ERROR_NONE, "Hardware init failed!");
    pc.printf("Program Starting");
    
    // Game state variables
    int num_remain_city; // number of cities currently on the landscape
    int player_alive; // 1 if alive, 0 if hit
    
    startScreen();

    // Timer to measure game update speed (secondarily used to generate random seed)
    Timer t;
    int dt; // delta time
    set_random_seed(t); // Already implemented.
    
    //Initialization functions (already implemented)
    city_landscape_init(num_city_g);
    missile_init();
    player_init();        
    
    while(1)
    {   
        // check high score


            
        
        level = 1 + missilescore / 10;
        if (level > 1 && missilescore % 10 == 0 && missilescore >= 10) {
            if (speed < 8) {
                speed++;
                set_missile_speed(speed);
                }
            if (interval > 7) {
                interval--;
                set_missile_interval(interval);
                }
            
            }
        uLCD.text_string("Level: ", 0, 1, FONT_7X8, 0xF31B97);
        char digit3 = level % 10 + '0';
        uLCD.text_char(digit3, 6, 1, 0xF31B97);
        uLCD.text_string("Score: ", 9, 1, FONT_7X8, 0xF31B97);
        digit3 = missilescore % 10 + '0';
        char digit2 = (missilescore / 10) % 10 + '0';
        char digit1 = missilescore / 100 + '0';
        if (missilescore >= 100) {
        uLCD.text_char(digit1, 15, 1, 0xF31B97);
        uLCD.text_char(digit2, 16, 1, 0xF31B97);
        uLCD.text_char(digit3, 17, 1, 0xF31B97);
        } else if (missilescore >= 10 && missilescore < 100) {
            uLCD.text_char(digit2, 15, 1, 0xF31B97);
            uLCD.text_char(digit3, 16, 1, 0xF31B97);
            } else {
                uLCD.text_char(digit3, 15, 1, 0xF31B97);
                }
        uLCD.text_string("Bullets: ", 0, 2, FONT_7X8, GREEN);
        digit3 = bullets % 10 + '0';
        digit2 = bullets / 10 + '0';
        if (bullets >= 0) {
            uLCD.text_char(digit2, 8, 2, GREEN);
            uLCD.text_char(digit3, 9, 2, GREEN);
        } else {
            uLCD.text_char(digit3, 10, 2, GREEN);
            } 
        

        
        t.start();
        // Generate new missiles and draw all active missiles at current 
        // positions. (Already implemented.)
        missile_generator();
        
        // Draw all active anti-missiles (aka player missiles) at current 
        // positions. (Already implemented.)
        player_missile_draw();
        
        // Detect missile collisions with city/land and update status.
        // You need to implement this (see specification below).
        num_remain_city = city_landscape_update();
        
        // Detect missile collision with player aircraft.
        // You need to implement this (see specification below).
        player_alive = was_player_hit(); //returns 0 if player is hit; else 1
        
        // Detect missile collisions with player anti-missiles.
        // You need to implement this (see specification below).
        missile_contact();
    
        // You must complete the implementation of this function in hardware.cpp:
        inputs = read_inputs();

        // You must write the code to dispatch to the correct action (e.g., 
        // player_moveLeft/Right, player_fire, etc.) based on the inputs read.
        // You must also implement player_moveLeft/moveRight/fire (see player
        // module).
        if (inputs.x > .55) {
            player_moveLeft();
        }
        if (inputs.x < -.55) { 
            player_moveRight();
        }
        if (inputs.joystickButton == 1) {
            player_fire();
        }
        if (inputs.touchpadValue) { // advanced feature: you start with 30 turret bullets that each destroy an enemy missile up
            DLinkedList* dll = (DLinkedList*) get_missile_list();
            LLNode* node = (LLNode*) dll->head;
            while (node != NULL) {
            MISSILE* missile = (MISSILE*) node->data;
            if (missile != NULL && missile->status != MISSILE_EXPLODED) {
                missile->status = MISSILE_EXPLODED; 
                bullets--;
                break;
            } else {
                LLNode* nextnode = node->next;
                MISSILE* nextmissile = (MISSILE*) nextnode->data;
            }
            
        }
               }
               
        if (inputs.b3 != 0 && inputs.b1 != 0){
            missilescore = missilescore / 10 * 10 + 10;
            }
        // GAMEOVER
        if (player_alive == 0 || num_remain_city == 0) {
            FILE* fp = fopen("/sd/dir/sdtest.txt", "r"); 
            // check for new high score on gameover since total missilescore has been calculated and player will always die.
            if (fp == NULL) {
                fclose(fp);
                FILE *fp = fopen("/sd/dir/sdtest.txt", "w");
                fprintf(fp, "%d", missilescore);
                fclose(fp);
            } else {
                while (!feof(fp)) {
                    fgets(buffer, 64, fp);
                    currenthighscore = atoi(buffer);
                }
                fclose(fp);
                if (missilescore > currenthighscore) {
                    FILE* fp = fopen("/sd/dir/sdtest.txt", "w");
                    fprintf(fp, "%d", missilescore);
                    fclose(fp);
                }
            }
            uLCD.cls();
            uLCD.text_bold(ON);
            uLCD.text_string("GAME OVER", 4, 7, FONT_7X8, RED);
            uLCD.text_bold(ON);
            uLCD.text_string("Level: ", 0, 1, FONT_7X8, 0xF31B97);
            uLCD.text_bold(ON);
            char digit3 = level % 10 + '0';
            uLCD.text_bold(ON);
            uLCD.text_char(digit3, 6, 1, 0xF31B97);
            uLCD.text_bold(ON);
            uLCD.text_string(" Your Score: ", 9, 1, FONT_7X8, 0xF31B97);
            uLCD.text_bold(ON);
            digit3 = missilescore % 10 + '0';
            char digit2 = (missilescore / 10) % 10 + '0';
            char digit1 = missilescore / 100 + '0';
            FILE* fp2 = fopen("/sd/dir/sdtest.txt", "r");
            while (!feof(fp)) {
                    fgets(buffer, 64, fp);
                    currenthighscore = atoi(buffer);
                }
            uLCD.locate(3, 6);
            uLCD.text_string("High Score: ", 9, 1, FONT_7X8, 0xF31B97);
            uLCD.printf("%d", currenthighscore);
            if (missilescore >= 100) {
                uLCD.text_char(digit1, 15, 1, 0xF31B97);
                uLCD.text_char(digit2, 16, 1, 0xF31B97);
                uLCD.text_char(digit3, 17, 1, 0xF31B97);
            } else if (missilescore >= 10 && missilescore < 100) {
                uLCD.text_char(digit2, 15, 1, 0xF31B97);
                uLCD.text_char(digit3, 16, 1, 0xF31B97);
            } else {
                uLCD.text_char(digit3, 15, 1, 0xF31B97);
                }
            break;
            }
            
        // Compute update time to control timing of the game loop.
        // (Already implemented... you're welcome.)
        t.stop();
        dt = t.read_ms();
        if (dt < 100) wait_ms(100 - dt);
    }
    pc.printf("out of main loop\n");
    
    DLinkedList* missilelist = (DLinkedList*) get_missile_list();
    destroyList(missilelist);
    PLAYER player = player_get_info();
    DLinkedList* playermissilelist = (DLinkedList*) player.playerMissiles;
    destroyList(playermissilelist);
    return 0;
}

/** Detect whether any missile has hit a city and if so, call 
    city_demolish (hint: city_get_info may be useful).
    Also, if any missile has hit a city or the landscape,
    mark the missile's status as MISSILE_EXPLODED 
    which will cue the missile's deletion and erasure from the screen 
    on the next missile_generator call.
    @return  Number of remaining cities.
*/
int city_landscape_update(void){
    DLinkedList* emlist = get_missile_list();
    LLNode* emcurrnode = emlist->head;
    int limit = num_city_g;
    while (emcurrnode != NULL) {
        pc.printf("NUMBER OF CITIES REMAINING: %d", cities);
        MISSILE* emcurr = (MISSILE*) emcurrnode->data;
        if (emcurr->y >= 123) {
            emcurr->status = MISSILE_EXPLODED;
        } 
        for (int i = 0; i < limit; i++) {
            CITY c = city_get_info(i);
            if (c.status != DEMOLISHED) {
            for (int j = 0; j <= c.width; j++) {
                if (emcurr->x == c.x + j && emcurr->y == c.y - c.height) {
                    city_demolish(i);
                    uLCD.circle(emcurr->x, emcurr->y, 3, 0xFFF633);
                    uLCD.circle(emcurr->x, emcurr->y, 6, 0xFF3333);
                    uLCD.filled_circle(emcurr->x, emcurr->y, 10, 0xFFB233);
                    wait(.1);
                    uLCD.filled_circle(emcurr->x, emcurr->y, 10, BACKGROUND_COLOR);
                    wait(.2);
                    uLCD.pixel(emcurr->x + 2, emcurr->y + 2, 0xF3D51B);
                    uLCD.pixel(emcurr->x - 2, emcurr->y - 2, 0x1CE9F3);
                    uLCD.pixel(emcurr->x + 2, emcurr->y - 2, 0xF3D51B);
                    uLCD.pixel(emcurr->x - 2, emcurr->y + 2, 0x1CE9F3);
                    uLCD.pixel(emcurr->x + 9, emcurr->y + 4, 0xF3D51B);
                    uLCD.pixel(emcurr->x - 4, emcurr->y - 3, 0xF3D51B);
                    uLCD.pixel(emcurr->x + 3, emcurr->y - 4, 0x1CE9F3);
                    uLCD.pixel(emcurr->x - 9, emcurr->y + 3, 0xF3D51B);
                    uLCD.pixel(emcurr->x + 6, emcurr->y + 8, 0xF3D51B);
                    uLCD.pixel(emcurr->x - 7, emcurr->y - 6, 0x1CE9F3);
                    uLCD.pixel(emcurr->x + 9, emcurr->y - 4, 0xF3D51B);
                    uLCD.pixel(emcurr->x - 2, emcurr->y + 9, 0x1CE9F3);
                    uLCD.pixel(emcurr->x - 3, emcurr->y + 9, 0xF3D51B);
                    uLCD.pixel(emcurr->x + 9, emcurr->y + 2, 0xF3D51B);
                    uLCD.pixel(emcurr->x - 4, emcurr->y - 6, 0x1CE9F3);
                    uLCD.pixel(emcurr->x + 6, emcurr->y - 6, 0xF3D51B);
                    uLCD.pixel(emcurr->x - 3, emcurr->y + 9, 0x1CE9F3);
                    wait(.2);
                    uLCD.filled_circle(emcurr->x, emcurr->y, 10, BACKGROUND_COLOR);
                    draw_landscape();
                    emcurr->status = MISSILE_EXPLODED;
                    cities--;
                } 
               }     for (int l = 0; l <= c.height - 1; l++) {
                if ((emcurr->x == c.x && emcurr->y == c.y - l) || (emcurr->x == c.x + c.width && emcurr->y == c.y - l)) {
                    city_demolish(i);
                    uLCD.circle(emcurr->x, emcurr->y, 3, 0xFFF633);
                    uLCD.circle(emcurr->x, emcurr->y, 6, 0xFF3333);
                    uLCD.filled_circle(emcurr->x, emcurr->y, 10, 0xFFB233);
                    wait(.1);
                    uLCD.filled_circle(emcurr->x, emcurr->y, 10, BACKGROUND_COLOR);
                    wait(.2);
                    uLCD.pixel(emcurr->x + 2, emcurr->y + 2, 0x1CE9F3);
                    uLCD.pixel(emcurr->x - 2, emcurr->y - 2, 0xF3D51B);
                    uLCD.pixel(emcurr->x + 2, emcurr->y - 2, 0x1CE9F3);
                    uLCD.pixel(emcurr->x - 2, emcurr->y + 2, 0xF3D51B);
                    uLCD.pixel(emcurr->x + 9, emcurr->y + 4, 0xF3D51B);
                    uLCD.pixel(emcurr->x - 4, emcurr->y - 3, 0x1CE9F3);
                    uLCD.pixel(emcurr->x + 3, emcurr->y - 4, 0xF3D51B);
                    uLCD.pixel(emcurr->x - 9, emcurr->y + 3, 0x1CE9F3);
                    uLCD.pixel(emcurr->x + 6, emcurr->y + 8, 0xF3D51B);
                    uLCD.pixel(emcurr->x - 7, emcurr->y - 6, 0xF3D51B);
                    uLCD.pixel(emcurr->x + 9, emcurr->y - 4, 0x1CE9F3);
                    uLCD.pixel(emcurr->x - 2, emcurr->y + 9, 0xF3D51B);
                    uLCD.pixel(emcurr->x - 3, emcurr->y + 9, 0xF3D51B);
                    uLCD.pixel(emcurr->x + 9, emcurr->y + 2, 0x1CE9F3);
                    uLCD.pixel(emcurr->x - 4, emcurr->y - 6, 0xF3D51B);
                    uLCD.pixel(emcurr->x + 6, emcurr->y - 6, 0xF3D51B);
                    uLCD.pixel(emcurr->x - 3, emcurr->y + 9, 0x1CE9F3);
                    
                    wait(.2);
                    uLCD.filled_circle(emcurr->x, emcurr->y, 10, BACKGROUND_COLOR);
                    draw_landscape();
                    emcurr->status = MISSILE_EXPLODED;
                    cities--;
                }
            }
                    
                    
            }

                }
           emcurrnode = emcurrnode->next;
        }
    return cities;
}

/** Detect whether any missile has hit the player aircraft and if so, call 
    player_destroy (hint: player_get_info may be useful) and mark the 
    missile's status as MISSILE_EXPLODED which will cue the missile's deletion
    and erasure from the screen on the next missile_generator call.
    @return 1 if the player aircraft was hit and 0 otherwise.
*/
int was_player_hit(){
    PLAYER player = (PLAYER) player_get_info();
    DLinkedList* dll = (DLinkedList*) get_missile_list();
    LLNode* missilecurr = (LLNode*) dll->head;
    while (missilecurr != NULL) {
        MISSILE* missile = (MISSILE*) missilecurr->data;
        for (int i = player.x; i < player.x + player.width; i++) {
            for (int j = player.y - player.height - 2; j < player.y; j++) {
                if (missile->x == i && missile->y == j) {
                    missile->status = MISSILE_EXPLODED;
                    player_destroy();
                    return 0;
                    }
                }
            } 
            missilecurr = missilecurr->next;
        }
    return 1;
}



/** Detect whether any missile has hit any player missile and if so, 
    mark the status of both the missile and the player missile as 
    MISSILE_EXPLODED which will cue the missile's deletion and erasure 
    from the screen on the next missile_generator call.
*/
void missile_contact(void) {
    int radius = 10 - (level / 2) * 2;
    DLinkedList* pmlist = player_get_info().playerMissiles;
    DLinkedList* emlist = get_missile_list();
    LLNode* emcurrnode = emlist->head;
    LLNode* pmcurrnode = pmlist->head;
    while (pmcurrnode != NULL) {
         PLAYER_MISSILE* pmcurr = (PLAYER_MISSILE*) pmcurrnode->data;
        while (emcurrnode != NULL) {
            MISSILE* emcurr = (MISSILE*) emcurrnode->data;
            double o = ((emcurr->x - pmcurr->x) * (emcurr->x - pmcurr->x)) + ((emcurr->y - pmcurr->y) * (emcurr->y - pmcurr->y));
            double d = sqrt(o);
            if (d <= radius) {
                FILE* fp = fopen("/sd/dir/sdtest.txt", "r");
                missilescore++;
                if (fp == NULL) {
                    uLCD.color(0xF4599D);
                    uLCD.locate(2,6);
                    uLCD.printf("NEW HIGHSCORE");
                    wait(1);
                    fclose(fp);
                } else {
                    while (!feof(fp)) {
                        fgets(buffer, 64, fp);
                        currenthighscore = atoi(buffer);
                }
                if (missilescore > currenthighscore) {
                    uLCD.color(0xF4599D);
                    uLCD.locate(2,6);
                    uLCD.printf("NEW HIGHSCORE");
                    wait(1);
                }
                fclose(fp);
            }
            uLCD.filled_circle(emcurr->x, emcurr->y, 10, 0xFFA500);
            uLCD.filled_circle(emcurr->x, emcurr->y, 10, BACKGROUND_COLOR);
            player_missile_draw();
            pmcurr->status = PMISSILE_EXPLODED;
            player_missile_draw();
            emcurr->status = MISSILE_EXPLODED;
            player_missile_draw();
            emcurrnode = emcurrnode->next;  
            } else {
                emcurrnode = emcurrnode->next;
            }
        } 
        pmcurrnode = pmcurrnode->next;
    }
}

// generate random positions for missiles to spawn
void set_random_seed(Timer t) {
    GameInputs inputs; 
    t.start();
    uLCD.printf("\n");
    uLCD.color(RED);
    uLCD.printf("Press any button \n to start.\n");
    while(1) {
        inputs = read_inputs();
        if (inputs.b1 || inputs.b2 || inputs.b3) break;
    }
    uLCD.cls();
    t.stop();
    int seed = t.read_ms();    
    srand(seed);
}

