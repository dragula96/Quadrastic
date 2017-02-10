/*
  Quadrastic
  version: 1.01
  February  09, 2017
  Copyright (C) 2015 David Martinez
  All rights reserved.
  programmed by: David Martinez
  Graphics by: DarKaoz

  This game is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
*/

#include <Arduboy2.h>
#include <ArduboyPlaytune.h>
#include "bitmaps.h" //load bitmaps
#include "bgm.h" //load music

// Make an instance of arduboy used for many functions
Arduboy2 arduboy;
ArduboyPlaytune tunes(arduboy.audio.enabled);
// Variables for your game go here.
byte playerX;
byte playerY;
byte score;
byte highScore;
byte soundOptions;
bool playerIntroPlayed;
byte delayPressA;
enum GAME_STATE {TITLE_SCREEN, GAME, GAME_OVER, PAUSED, SPLASH_SCREEN};
GAME_STATE gameState;
#define PLAYER_SPEED 2
#define PLAYER_SIZE 6
#define ENEMY_SIZE 6
#define TARGET_SIZE 1
#define MAX_ENEMIES 70

// TARGET STRUCT
struct Target {
  int x;
  int y;
  byte Size;
  void init() {
    Size = 15;
    x =  1 + random(1, 32) * 4;
    y =  1 + random(1, 16) * 4 ;
    Rect scoreRect = {.x = WIDTH / 2 - 8, .y = 0, .width = 16, .height = 10};
    Rect playerRect = {.x = playerX, .y = playerY, .width = PLAYER_SIZE, .height = PLAYER_SIZE};

    bool badSpawn = true; //lets asume its always a bad spawn (untill it isnt)
    while (badSpawn) {
      Point targetPoint = {.x = x, .y = y};
      if ((arduboy.collide(targetPoint, playerRect)) || (arduboy.collide(targetPoint, scoreRect)) ) {
        x =  1 + random(1, 32) * 4;
        y =  1 + random(1, 16) * 4 ;
        badSpawn = true;
      } else {
        badSpawn = false;
      }

    }

  }
  void Update() {
    if (Size > 1) {
      Size --;
    }

  }
};

//ENEMY STRUCT
struct Enemy {
  byte x = 0;
  byte y = 0;
  byte  xSpeed = 0;
  byte  ySpeed = 0;
  byte  Speed = 0;
  bool isNegative = false;
  void init() {
    xSpeed = 0;
    ySpeed = 0;
    x =  random(2, 31) * 4;
    y = random(2, 15) * 4;
    Speed = 1;
    isNegative = false;
    byte leftRightorUpDown = random(0, 2);

    Rect playerRect = {.x = playerX, .y = playerY, .width = PLAYER_SIZE, .height = PLAYER_SIZE};

    bool badSpawn = true; //lets asume its always a bad spawn (untill it isnt)
    while (badSpawn) {
      Rect enemyRect = {.x = x, .y = y, .width = ENEMY_SIZE, .height = ENEMY_SIZE};
      if (arduboy.collide(enemyRect, playerRect)) {
        x =  random(2, 31) * 4;
        y = random(2, 15) * 4;
        badSpawn = true;
      } else {
        badSpawn = false;
      }

    }

    //IF LEFTRIGHT WINS
    if (leftRightorUpDown == 0) {
      xSpeed = Speed;
      ySpeed = 0;

    } else {
      ySpeed = Speed;
      xSpeed = 0;
    }

    byte randNegative = random(0, 2);
    if (randNegative > 0) {
      isNegative = false;
    } else {
      isNegative = true;
    }
  }
  void update() {

    if (isNegative) {
      x = x - xSpeed;
      y = y - ySpeed;

      if ((x < 1) || ( y < 1)) {
        isNegative = false;
      }
    } else {
      x = x + xSpeed;
      y = y + ySpeed;
      if ((x > WIDTH - ENEMY_SIZE) || (y > HEIGHT - ENEMY_SIZE)) {
        isNegative = true;
      }

    }


  }
};

Enemy enemy[MAX_ENEMIES];
Target target;

// This function runs once in your game.
// use it for anything that needs to be set only once in your game.

void setup() {
  //initiate arduboy instance
  arduboy.begin();

  // here we set the framerate
  arduboy.setFrameRate(30);
  soundOptions = 2; //0 = mute, 1 = SFX only, 2 = both music and sound effects.
  // audio setup
  tunes.initChannel(PIN_SPEAKER_1);
#ifndef AB_DEVKIT
  // if not a DevKit
  tunes.initChannel(PIN_SPEAKER_2);
#else
  // if it's a Devkit
  tunes.initChannel(PIN_SPEAKER_1); // use the same pin for both channels
  tunes.toneMutesScore(true);       // mute the score when a tone is sounding
#endif

  highScore = 0;
  createEnemies();

  gameState = SPLASH_SCREEN;
}

// our main game loop, this runs once every cycle/frame.
// this is where our game logic goes.
void loop() {
  // pause render until it's time for the next frame
  if (!(arduboy.nextFrame()))
    return;

  switch (gameState) {

    case SPLASH_SCREEN:
      //DMG LOGO
      arduboy.clear();
      //DRAW IMAGE
      arduboy.drawBitmap(0, 0, dmgLogoImg, 128, 64, WHITE);
      arduboy.display();
      delay(2000);
      //DARKAOZ LOGO
      arduboy.clear();
      //DRAW IMAGE
      arduboy.drawBitmap(0, 0, darkaozImg, 128, 64, WHITE);
      arduboy.display();
      delay(2000);

      gameState = TITLE_SCREEN;
      break;
    case TITLE_SCREEN:

      //ANIMATE TITLE WORDS
      for (byte i = 64; i > 10; i--) {
        arduboy.clear();
        //DRAW IMAGE
        arduboy.drawBitmap(WIDTH / 2 - 52, i, titleImg, 109, 16, WHITE);
        arduboy.display();
        delay(30);
      }

      boolean exitTitle;
      exitTitle = false;
      while (!exitTitle) {

        arduboy.clear();
        arduboy.drawBitmap(WIDTH / 2 - 52, 10, titleImg, 109, 16, WHITE);

        if (soundOptions > 0) {
          arduboy.drawBitmap(WIDTH - 20, HEIGHT - 14, speakerImg, 16, 16, WHITE);
        }
        if (soundOptions == 0) {
          arduboy.drawBitmap(WIDTH - 36, HEIGHT - 12, muteImg, 32, 8, WHITE);
        }
        if (soundOptions == 2) {
          arduboy.drawBitmap(WIDTH - 36, HEIGHT - 14, noteImg, 16, 16, WHITE);
        }

        delayPressA += 2;
        if (delayPressA < 128) {
          arduboy.drawBitmap(WIDTH / 2 - 32, HEIGHT / 2 + 10, pressAImg, 64, 8, WHITE);
        }
        arduboy.display();

        // if the  B button is pressed TUGGLE SOUND
        arduboy.pollButtons();
        if (arduboy.justPressed(B_BUTTON)) {
          soundOptions ++;
          if (soundOptions > 2) {
            soundOptions = 0;
          }
          if (soundOptions == 0) {

            arduboy.audio.off();
          } else {
            arduboy.audio.on();
          }
        }
        // if the  A button is pressed start game;
        if (arduboy.pressed(A_BUTTON)) {
          arduboy.initRandomSeed();

          setTarget();
          score = 0;
          // set player to the middle of the screen
          playerX = (WIDTH / 2) - (PLAYER_SIZE / 2);
          playerY = (HEIGHT / 2) - (PLAYER_SIZE / 2);
          exitTitle = true;
          playerIntroPlayed = false;
          gameState = GAME;
        }

      }
      break;

    case GAME_OVER:

      arduboy.clear();
      //DRAW GAMEOVER
      arduboy.drawBitmap(0, HEIGHT / 2 - 16, gameOverImg, 128, 32, WHITE);
      //PRINT HIGH SCORE
      arduboy.setCursor(WIDTH / 2 - 40, 1);
      arduboy.print("HIGH SCORE:");
      arduboy.print(highScore);

      arduboy.setCursor(WIDTH / 2 - 23, HEIGHT / 2 + 23 );
      arduboy.print("SCORE:");
      arduboy.print(score);

      arduboy.display();
      delay(3000);
      gameState = TITLE_SCREEN;
      break;

    case PAUSED:


      arduboy.pollButtons();

      arduboy.setCursor(WIDTH / 2 - 23, HEIGHT / 2 - 3);
      arduboy.print("PAUSED!");
      arduboy.display();

      if (arduboy.justPressed(B_BUTTON)) {

        gameState = GAME;
      }
      break;

    case GAME:
      //PLAYER INTRO EFFECT
      if (!playerIntroPlayed) {
        byte x1 = 0;
        byte x2 = WIDTH - PLAYER_SIZE;
        while (x1 < playerX) {
          x1++;
          x2--;
          arduboy.clear();
          //DRAW BACKGROUND
          arduboy.drawRect(0, 0, WIDTH, HEIGHT);
          arduboy.fillRect(x1, playerY, PLAYER_SIZE, PLAYER_SIZE);
          arduboy.fillRect(x2, playerY, PLAYER_SIZE, PLAYER_SIZE);
          arduboy.display();
          delay(2);
        }
        playerIntroPlayed = true;
      }

      // we clear our screen to black
      arduboy.clear();
      checkButtons();

      //check collissions
      checkIfScored();

      //We print score to screen
      arduboy.setCursor(WIDTH / 2 - 5, 3);
      arduboy.print(score);

      updateEnemies();
      //DRAW BACKGROUND
      arduboy.drawRect(0, 0, WIDTH, HEIGHT);
      //DRAW PLAYER
      arduboy.drawRect(playerX, playerY, PLAYER_SIZE, PLAYER_SIZE);
      //DRAW TARGET
      arduboy.drawCircle(target.x, target.y, target.Size);
      arduboy.drawPixel(target.x, target.y, WHITE);
      target.Update();
      // then we finaly we tell the arduboy to display what we just wrote to the display.
      arduboy.display();

      // play the tune if we aren't already
      if ((!tunes.playing()) && (soundOptions == 2) && (gameState != GAME_OVER)) {
        tunes.playScore(bgm);
      }

      //END GAME SWITCH CASE
      break;
  }
}

void checkButtons() {
  // the next couple of lines will deal with checking if the D-pad buttons
  // are pressed and move our text accordingly.
  // We check to make sure that x and y stay within a range that keeps the
  // text on the screen.

  // if the right button is pressed
  if (arduboy.pressed(RIGHT_BUTTON) && (playerX < WIDTH - PLAYER_SIZE - 1)) {
    playerX += PLAYER_SPEED;
  }

  // if the left button is pressed
  if (arduboy.pressed(LEFT_BUTTON) && (playerX > 1)) {
    playerX -= PLAYER_SPEED;
  }

  // if the up button is pressed
  if ((arduboy.pressed(UP_BUTTON) && (playerY > 1))) {
    playerY -= PLAYER_SPEED;
  }

  // if the down button  is pressed
  if ((arduboy.pressed(DOWN_BUTTON) ) && (playerY < HEIGHT - PLAYER_SIZE - 1)) {
    playerY += PLAYER_SPEED;
  }

  // if the B button is pressed pause
  arduboy.pollButtons();
  if (arduboy.justPressed(B_BUTTON)) {

    gameState = PAUSED;
  }
}

void setTarget() {
  target.init();

}
void checkIfScored() {

  Rect playerRect = {.x = playerX, .y = playerY, .width = PLAYER_SIZE, .height = PLAYER_SIZE};
  Point targetPoint = {.x = target.x, .y = target.y};

  if (arduboy.collide(targetPoint, playerRect)) {
    setTarget();
    score ++;
    //CHECK IF ENEMY SPAWNS DIRECTLY ON PLAYER, IF SO THEN
    //RE RANDOMIZE ENEMY TO GIVE PLAYER A FAIR CHANCE
    Rect playerRect = {.x = playerX, .y = playerY, .width = PLAYER_SIZE, .height = PLAYER_SIZE};
    Rect enemyRect = {.x = enemy[score - 1].x, .y = enemy[score - 1].y, .width = ENEMY_SIZE, .height = ENEMY_SIZE};
    if (arduboy.collide(enemyRect, playerRect)) {
      // gameState = PAUSED;
      enemy[score - 1].init();
    }

    tunes.tone(1397, 300);
    if (score > highScore) {
      highScore = score;
    }

  }
}
void updateEnemies() {
  if (score > 0) {
    //CREATE PLAYER RECT FOR COLLISSION CHECKING
    Rect playerRect = {.x = playerX + 2, .y = playerY + 2, .width = PLAYER_SIZE - 4, .height = PLAYER_SIZE - 4};

    //UPDATE, DRAW, AND COLLISION CHECK ONLY ACTIVE ENEMIES
    for (byte i = 0; i < score; i++) {
      enemy[i].update();
      //DRAW ENEMY
      arduboy.drawBitmap(enemy[i].x, enemy[i].y, enemyImg, ENEMY_SIZE, ENEMY_SIZE, WHITE);
      //CHECK ENEMY COLLISIONS WITHIN THIS LOOP TO OPTIMIZE FRAMERATE
      Rect enemyRect = {.x = enemy[i].x, .y = enemy[i].y, .width = ENEMY_SIZE, .height = ENEMY_SIZE};
      //CHECK IF GAMEOVER
      if (arduboy.collide(enemyRect, playerRect)) {
        //stop music
        if (tunes.playing())
          tunes.stopScore();

        //PAUSE FOR VISUAL
        delay(1000);
        gameState = GAME_OVER;

      }
    }
  }

}

void createEnemies() {
  for (byte i = 0; i < MAX_ENEMIES; i++) {
    enemy[i].init();
  }
}
