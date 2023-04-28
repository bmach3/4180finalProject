//=================================================================
// Implementation for the player module
//
// Copyright 2021 Georgia Tech.  All rights reserved.
// The materials provided by the instructor in this course are for
// the use of the students currently enrolled in the course.
// Copyrighted course materials may not be further disseminated.
// This file must not be made publicly available anywhere.
//==================================================================
#include "globals.h"
#include "player_private.h"

PLAYER player; // structure of player

PLAYER player_get_info(void){ // getter for user to acquire info without accessing structure
    return player;
}

// initialize the player's position, missile status, draw player, 
void player_init(void) {    
    player.x = PLAYER_INIT_X; player.y = PLAYER_INIT_Y; player.status = ALIVE;    
    player.playerMissiles = create_dlinkedlist();    
    player.delta = PLAYER_DELTA;
    player.width = PLAYER_WIDTH; 
    player.height = PLAYER_HEIGHT;
    player_draw(PLAYER_COLOR);
}

// move player PLAYER_DELTA pixels to the left, except if it would go off screen.
// In main(), we have already used the inputs of the accelerometer to call this method.
void player_moveLeft(void) { 
  // Checks to make sure we do not go off of the screen and moves by erasing and re-drawing.
  if ((player.x - player.delta) >= 0) {
    player_draw(BACKGROUND_COLOR);
    player.x -= player.delta;
    player_draw(PLAYER_COLOR);
  }
}

// move player PLAYER_DELTA pixels to the right, except if it would go off screen.
// In main(), we have already used the inputs of the accelerometer to call this method.
void player_moveRight(void) {
  // Checks to make sure we do not go off of the screen and moves by erasing and re-drawing.
  if ((player.x + player.width + player.delta <= 127)) {
    player_draw(BACKGROUND_COLOR);
    player.x += player.delta;
    player_draw(PLAYER_COLOR);
  }
}

// generate an active missile to shoot 
void player_fire() {
  // Allocates memory on the heap for the PLAYER_MISSILE structure and initializes its fields.
  // We also add the missile to the missle list of the player.
  PLAYER_MISSILE* newMissile = (PLAYER_MISSILE*)malloc(sizeof(PLAYER_MISSILE));
  newMissile->x = player.x + (player.width / 2);
  newMissile->y = player.y - player.height - 1;
  newMissile->status = PMISSILE_ACTIVE;
  insertHead(player.playerMissiles, newMissile);
  
}

// draw/updates the line of any active missiles, "erase" deactive missiles
void player_missile_draw(void) {
  PLAYER_MISSILE* playerMissile;
  LLNode* current = player.playerMissiles->head;
  while(current){
    playerMissile = (PLAYER_MISSILE*)current->data;                    
    if(playerMissile->status == PMISSILE_EXPLODED) {
      //pc.printf("pmd:exploded\n");
      uLCD.line(playerMissile->x, player.y-player.delta, playerMissile->x, playerMissile->y, BACKGROUND_COLOR);

      // Below is the displaying a circle indicating a collision has occured.
      uLCD.circle(playerMissile->x, playerMissile->y, 5, 65535);
      int i = 1001;
      // Has a while loop to show the circle for a little and then erases the explosion.
      while (i) {
        if (i == 1) {
          uLCD.circle(playerMissile->x, playerMissile->y, 5, BACKGROUND_COLOR);
        }
        i--;
      }
      //pc.printf("deleting node...\n");
      deleteNode(player.playerMissiles, current);
      //pc.printf("node deleted.\n");
    }
    else { // update missile position
      playerMissile->y -= PLAYER_MISSILE_SPEED;
      if (playerMissile->y < 0) {
        uLCD.line(playerMissile->x, player.y-player.delta, playerMissile->x, 0, BACKGROUND_COLOR);
        // Remove from list   
        // pc.printf("deleting node...\n");
        deleteNode(player.playerMissiles, current);
        // pc.printf("node deleted.\n");             
      }
      else {
        // draw missile
        uLCD.line(playerMissile->x, playerMissile->y+PLAYER_MISSILE_SPEED, playerMissile->x, playerMissile->y, PLAYER_MISSILE_COLOR);
      }
    }  
    current = current->next;              
  }
}
    

// ==== player_private.h implementation ====
void player_draw(int color) {
    uLCD.filled_rectangle(player.x, player.y, player.x+player.width, player.y+player.height, color); 
    uLCD.filled_rectangle(player.x+player.delta, player.y-player.delta, player.x+player.width-player.delta, player.y+player.height, color);
}

// destroy and "erase" the player off the screen. change status to DESTROYED
void player_destroy() {
    player_draw(BACKGROUND_COLOR);
    player.status = DESTROYED;
    uLCD.circle(player.x, player.y, 3, 0xFFF633);
    uLCD.circle(player.x, player.y, 6, 0xFF3333);
    uLCD.filled_circle(player.x, player.y, 10, 0xFFB233);
    wait(.05);
    uLCD.filled_circle(player.x, player.y, 10, BACKGROUND_COLOR);
    wait(.1);
    uLCD.pixel(player.x + 2, player.y + 2, 0xFF632D);
    uLCD.pixel(player.x - 2, player.y - 2, 0xFF6050);
    uLCD.pixel(player.x + 2, player.y - 2, 0xFF632D);
    uLCD.pixel(player.x - 2, player.y + 2, 0xF1EEED);
    uLCD.pixel(player.x + 9, player.y + 4, 0xFF632D);
    uLCD.pixel(player.x - 4, player.y - 3, 0xFF6050); 
    uLCD.pixel(player.x + 3, player.y - 4, 0xFF632D);
    uLCD.pixel(player.x - 9, player.y + 3, 0xF3D51B);
    uLCD.pixel(player.x + 6, player.y + 8, 0xF3D51B);
    uLCD.pixel(player.x - 7, player.y - 6, 0xF1EEED);
    uLCD.pixel(player.x + 9, player.y - 4, 0xF3D51B); 
    uLCD.pixel(player.x - 2, player.y + 9, 0xF1EEED);
    uLCD.pixel(player.x - 3, player.y + 9, 0xF3D51B);
    uLCD.pixel(player.x + 9, player.y + 2, 0xFF6050);
    uLCD.pixel(player.x - 4, player.y - 6, 0xF1EEED);
    uLCD.pixel(player.x + 6, player.y - 6, 0xFF6050);
    uLCD.pixel(player.x - 3, player.y + 9, 0xF1EEED);
    wait(.1);
    uLCD.filled_circle(player.x, player.y, 10, BACKGROUND_COLOR);
}