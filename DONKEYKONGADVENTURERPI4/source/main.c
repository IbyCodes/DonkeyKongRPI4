// DONKEY KONG ADVENTURE RPI4
// Mohammad Ibrahim Khan 
// Osama Hameed

#include <unistd.h>
#include <stdio.h>
#include <time.h>

#include "gpio.h"


#include "uart.h"

#include <stdlib.h>
#include <stdbool.h>

#include "fb.h"

#include "donkeykong.h"

/*
// ALL IMAGE LINKS/SOURCES USED:
https://mentalmars.com/wallpapers/donkey-kong-country-wallpaper/
https://pixelation.org/index.php?topic=36764.0
https://pngtree.com/freepng/banana-pixel-icon_8522677.html
https://dinopixel.com/pixel-art/wasp/11271
https://www.destructoid.com/the-troubled-life-and-times-of-the-kongs/
https://www.pngwing.com/en/free-png-zuyzc/download
https://iconscout.com/icon/replay-1780196
https://www.deviantart.com/sarahstudiosart/art/Dirt-Tile-1-458649206
https://opengameart.org/content/32x32-pixel-art-creatures-volume-3
https://opengameart.org/content/32x32-blocks-and-more
https://opengameart.org/content/dungeon-crawl-32x32-tiles
https://www.nicepng.com/downpng/u2q8a9u2w7o0e6u2_donkey-kong-png-transparent-donkey-kong/
*/

// general images used in all levels/ used in menus
#include "mainmenustart.h"
#include "mainmenuquit.h"
#include "gameoverretry.h"
#include "gameoverquit.h"
#include "liveslabel.h" // label for "LIVES:"
#include "heart.h"  // heart image for lives
#include "heartpack.h" // gain a live pack image
#include "gainsecondspack.h" // gain 5 seconds pack image
#include "invinciblepack.h" // become invincible for 3 seconds pack image
#include "banana.h"
#include "youwinagain.h"
#include "youwinquit.h"
#include "pausedrestart.h"
#include "pausedquit.h"

// all level one images
#include "grasstile.h" 
#include "levelOne.h"
#include "sampletile.h"
#include "pathtile.h"
#include "wasp.h"
#include "goomba.h"
#include "snakes.h"

// all level two images
#include "dirttile.h"
#include "golem.h"
#include "levelTwo.h"
#include "ant.h"
#include "pathtile2.h"
#include "scorpion.h"

// all level three images
#include "icetile.h"
#include "iceguard.h"
#include "icesquid.h"
#include "icebarrier.h"
#include "icemonster.h"
#include "levelThree.h"

// all level four iamges
#include "skulltile.h"
#include "firetile.h"
#include "reaper.h" // moving
#include "firemonster.h" // still
#include "reaper2.h" // still
#include "levelFour.h"


// SNES controller pins

#define LAT 9
#define CLK 11
#define DAT 10


#define GPIO_BASE 0xFE200000
#define CLO_REG 0xFE003004 // WAIT register

static unsigned *gpio = (unsigned*)GPIO_BASE;

// GPIO setup macros. Always use INP_GPIO(x) before using OUT_GPIO(x) or SET_GPIO_ALT(x,y) 

#define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
#define OUT_GPIO(g) *(gpio+((g)/10)) |= (1<<(((g)%10)*3))
#define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |= (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))



// SNES buttons

#define SNES_B 1
#define SNES_Y 2
#define SNES_SELECT 3
#define SNES_START 4
#define SNES_UP 5
#define SNES_DOWN 6
#define SNES_LEFT 7
#define SNES_RIGHT 8
#define SNES_A 9
#define SNES_X 10
#define SNES_TLEFT 11
#define SNES_TRIGHT 12



struct Player
{
    int xpos;
    int ypos;
    int height;
    int width;
    int invincible;
    int trackInvincible;
    unsigned char *image;
};


struct GameMap { // will be used in GameState structure
    int width;
    int height;
    int score;
    int lives;
    int timeLeft;
};


struct GameState {
    struct GameMap gameMap;
    int state;
    int changeLevel;
    int winCondition; // set to 0 if not won yet
    int loseCondition; // set to 0 if not lost yet
};

struct Object {
    int xpos;
    int ypos;
    int move; // 0 = obstacle can keep moving 1 = obstacle must stop moving 
    int width;
    int height;
    unsigned char *image;
    // add other attributes as needed
};




// Initializes a GPIO line

void Print_Message(char *msg_addr) { // prints a string to the screen (not really used in this game)
    uart_puts(msg_addr);
}

 

void Init_GPIO(int pin, int value) {
    if (pin == LAT && value == 1) { // For latch INPUT line
       INP_GPIO(LAT);
    }
    else if(pin == LAT && value == 0){ // For latch OUTPUT line
        OUT_GPIO(LAT);
    }
    else if (pin == CLK && value == 1){ // For clock INPUT line
        INP_GPIO(CLK);
    }
    else if (pin == CLK && value == 0){ // For clock OUTPUT line
        OUT_GPIO(CLK);
    }
    else if (pin == DAT && value == 1){ // For data INPUT Line
        INP_GPIO(DAT);
    }
}

 

// Write to GPIO Latch line
// NOTE: LATCH TRACKS WHAT BUTTONS ON THE CONTROLLER ARE PRESSED
void Write_Latch(int value){
    if (value) {
            *GPSET0 = 1 << LAT; // Set GPSET0 to 1 (required to write on the latch)
        } else {
            *GPCLR0 = 1 << LAT; // Set GPCLR0 to 0 (required to say that a controller button was NOT pressed)
        }
}


// Write to GPIO Clock line
   // NOTE: THE CLOCK TRACKS INTERVALS FOR WHAT BUTTON

void Write_Clock(int value){
    if (value) {
            *GPSET0 = 1 << CLK; // Set GPSET0 to 1
        } else {
            *GPCLR0 = 1 << CLK; // Set GPCLR0 to 0
        }
}


int Read_Data() {
    int pin = DAT; // pin#10 is the pin of the SNES controller (which is equal to DATA line)
    int value = 1 << pin; // aligning the pin 
    int result = (*GPLEV0 & value); // masking everything else
    if (result == 0) {
        return 0; 
    } else {
        return 1; 
    }
}


// Waits for a time interval, passed as a parameter 
void Wait(int time) {
    unsigned *clo = (unsigned*)CLO_REG;
    unsigned c = *clo + time; // in micro seconds
    while (*clo < c);
}

 
// Reading the SNES controller inputs 
int* Read_SNES() {
    int i;
    static int buttonsPressed[15]; // a integer array to store all of the buttons and keep track of what button has been pressed
    Write_Latch(1); 
    Write_Clock(1); 
    Wait(12); // signal to SNES to sample buttons
    Write_Latch(0); 
    for (i = 0; i < 16; i++) { // for all possible controller options
    Wait(6); // wait(6 micro seconds)
    Write_Clock(0); // falling edge of the clock
    Wait(6); // wait(6 micro seconds)
    buttonsPressed[i] = Read_Data(); // add to the array that will be returned
    Write_Clock(1); // rising edge; new cycle
    }
    return buttonsPressed; // returning the array of buttons (1= not pressed, 0= pressed)
}

struct Player kongPlayer; // structure for all character information
struct GameState gameState; // structure for all gameState information 
struct Object levelOneMovingObjects[2]; // 3 things for now
struct Object levelTwoMovingObjects[8]; // 9 things moving for now
struct Object levelThreeMovingObjects[6]; // 7 things for now
struct Object levelFourMovingObjects[16]; // 17 things for now
int signal; // signal for if theres been a collision with any moving enemy
int prevGamestate; // for when the game is paused

void clearScreen(){ // simply put, this function clears the screen 
    for (int i = 0; i < 1280; i++){
        for (int j=0; j<720; j++){
            drawPixel(i, j, 0xFF000000);
        }
    }
}

void drawMainMenuSTART() { // this draws the main menu where the cursor is on the START option

    myDrawImage(mainMenuStart.pixel_data,mainMenuStart.width, mainMenuStart.height,0,0, 0);
    gameState.state = 0;
}

void drawMainMenuQUIT() {// this draws the main menu where the cursor is on the QUIT option

    myDrawImage(mainMenuQuit.pixel_data,mainMenuQuit.width, mainMenuQuit.height,0,0, 0);
    gameState.state = 1;
}

void drawLives(){ // this function just draws out the hearts depending on number of lives left
     myDrawImage(livesLabel.pixel_data,livesLabel.width, livesLabel.height,750,200,0);
    int drawNextTo = 800;
    for (int i = 0; i< gameState.gameMap.lives; i++){
         myDrawImage(heart.pixel_data,heart.width, heart.height,drawNextTo,200,0);
         drawNextTo = drawNextTo + 50;
    }
}


void loseLive(){  // this function removes a heart from the screen if live lost
    int heartToDelete = 800;
    for (int i = 0; i< gameState.gameMap.lives; i++){
         heartToDelete = heartToDelete + 50;
    }
     for (int i = heartToDelete; i < heartToDelete+100; i++){
        for (int j=200; j<250; j++){
            drawPixel(i, j, 0xFF000000);
        }
    }
 
}

// function to convert an integer to a string (for the clock)
void intToStr(int num, char str[]) {
    int i = 0, sign = 0;
    if (num < 0) {
        sign = 1;
        num = -num;
    }
    while (num != 0) {
        str[i++] = num % 10 + '0';
        num /= 10;
    }
    if (sign)
        str[i++] = '-';
    str[i] = '\0';
    int len = i;
    for (i = 0; i < len / 2; i++) {
        char temp = str[i];
        str[i] = str[len - i - 1];
        str[len - i - 1] = temp;
    }
}

void drawPacks(){ // this function just draws packs depending on what level it is
    if (gameState.state == 2){
     for (int i = 0; i< LEVELONEWIDTH; i++){
        for (int j = 0; j<LEVELONEHEIGHT; j++){
            if (levelOne[j][i] == 6){
            myDrawImage(heartPack.pixel_data, heartPack.width, heartPack.height, i*32,j*32,1);
            }
            else if (levelOne[j][i] == 7){
            myDrawImage(gainSecondsPack.pixel_data, gainSecondsPack.width, gainSecondsPack.height, i*32,j*32,1);
            }
             else if (levelOne[j][i] == 8){
            myDrawImage(invinciblePack.pixel_data, invinciblePack.width, invinciblePack.height, i*32,j*32,1);
            }

        }
     }
    }
    else if (gameState.state == 3){
        for (int i = 0; i< LEVELTWOWIDTH; i++){
        for (int j = 0; j<LEVELTWOHEIGHT; j++){
            if (levelTwo[j][i] == 6){
            myDrawImage(heartPack.pixel_data, heartPack.width, heartPack.height, i*32,j*32,1);
            }
            else if (levelTwo[j][i] == 7){
            myDrawImage(gainSecondsPack.pixel_data, gainSecondsPack.width, gainSecondsPack.height, i*32,j*32,1);
            }
             else if (levelTwo[j][i] == 8){
            myDrawImage(invinciblePack.pixel_data, invinciblePack.width, invinciblePack.height, i*32,j*32,1);
            }
        }
     }
    }
     else if (gameState.state == 4){
        for (int i = 0; i< LEVELTHREEWIDTH; i++){
        for (int j = 0; j<LEVELTHREEHEIGHT; j++){
            if (levelThree[j][i] == 6){
            myDrawImage(heartPack.pixel_data, heartPack.width, heartPack.height, i*32,j*32,1);
            }
            else if (levelThree[j][i] == 7){
            myDrawImage(gainSecondsPack.pixel_data, gainSecondsPack.width, gainSecondsPack.height, i*32,j*32,1);
            }
             else if (levelThree[j][i] == 8){
            myDrawImage(invinciblePack.pixel_data, invinciblePack.width, invinciblePack.height, i*32,j*32,1);
            }
        }
     }
    }

     else if (gameState.state == 5){
        for (int i = 0; i< LEVELFOURWIDTH; i++){
        for (int j = 0; j<LEVELFOURHEIGHT; j++){
            if (levelFour[j][i] == 6){
            myDrawImage(heartPack.pixel_data, heartPack.width, heartPack.height, i*32,j*32,1);
            }
            else if (levelFour[j][i] == 7){
            myDrawImage(gainSecondsPack.pixel_data, gainSecondsPack.width, gainSecondsPack.height, i*32,j*32,1);
            }
             else if (levelFour[j][i] == 8){
            myDrawImage(invinciblePack.pixel_data, invinciblePack.width, invinciblePack.height, i*32,j*32,1);
            }
        }
     }
    }

}

void gameOverRETRY(){ // this draws the YOU LOSE where the cursor is on the RETRY option
if (gameState.loseCondition == 1){
    myDrawImage(gameOverRetry.pixel_data,gameOverRetry.width, gameOverRetry.height,0,0,0);
    gameState.state = 8;
}
}



void gameOverQUIT(){ // this draws the YOU LOSE where the cursor is on the QUIT option
    if (gameState.loseCondition == 1){
    myDrawImage(gameOverQuit.pixel_data,gameOverQuit.width, gameOverQuit.height,0,0,0);
    gameState.state = 9;
    }
}

void gameWonPLAY(){ // this draws the YOU WIN where the cursor is on the PLAY AGAIN option
    if (gameState.winCondition == 1){
    myDrawImage(youWinAgain.pixel_data,youWinAgain.width, youWinAgain.height,0,0,0);
    gameState.state = 6;
    char scoreBuf[10];
    drawString(600, 360, "Final Score: ", 0xA);
    intToStr (gameState.gameMap.score, scoreBuf);
    drawString(700, 360, scoreBuf, 0xA);
    }
}


void gameWonQUIT(){ // this draws the YOU WIN where the cursor is on the QUIT option
    if (gameState.winCondition == 1){
    myDrawImage(youWinQuit.pixel_data,youWinQuit.width, youWinQuit.height,0,0,0);
    gameState.state = 7;
    char scoreBuf[10];
    drawString(600, 360, "Final Score: ", 0xA);
    intToStr (gameState.gameMap.score, scoreBuf);
    drawString(700, 360, scoreBuf, 0xA);
    }
}

int randomMove(int time){ // to generate a random move for the moving enemy
    int i = 0;
    i = time % 6;
    if (i == 1||i == 3 || i == 4){
        return 1;
    }
    else{
        return 0;
    }

}

int randomMoveAllDirections(int time){ // to generate a random move in ALL possible directions (4 directions)
    int i = 0;
    i = time % 6;
    if (i == 1||i == 3||i == 4){
        return i;
    }
    else{
        return 2;
    }

}


void loadLevelOne(){ // to load all of level one for the first time
    clearScreen();  
    gameState.loseCondition = 0;
    gameState.winCondition = 0;
    levelOneMovingObjects[0].xpos = 1*32;
    levelOneMovingObjects[0].ypos = 9*32;
    levelOneMovingObjects[0].width = goomba.width;
    levelOneMovingObjects[0].height = goomba.height;
    levelOneMovingObjects[0].image = goomba.pixel_data;
    levelOneMovingObjects[0].move = 0;
    
    levelOneMovingObjects[1].xpos = 9*32;
    levelOneMovingObjects[1].ypos = 9*32;
    levelOneMovingObjects[1].width = goomba.width;
    levelOneMovingObjects[1].height = goomba.height;
    levelOneMovingObjects[1].image = goomba.pixel_data;
    levelOneMovingObjects[1].move = 0;

    levelOneMovingObjects[2].xpos = 16*32;
    levelOneMovingObjects[2].ypos = 2*32;
    levelOneMovingObjects[2].width = goomba.width;
    levelOneMovingObjects[2].height = goomba.height;
    levelOneMovingObjects[2].image = goomba.pixel_data;
    levelOneMovingObjects[2].move = 0;
    
    gameState.gameMap.width= LEVELONEWIDTH;
    gameState.gameMap.height = LEVELONEHEIGHT;
    gameState.changeLevel = 0;
    gameState.gameMap.lives = 4;
    gameState.gameMap.timeLeft = 120; // resetting the timer
    gameState.gameMap.score = 0; // REMEMBER TO CALCULATE SCORE AFTER EACH ROUND (AND DISPLAY IT)
    kongPlayer.image = donkeykong.pixel_data; // im also using the player structure so its easy to have all player data in one place
    kongPlayer.invincible = 0; 
    kongPlayer.xpos = 1*32;
    kongPlayer.ypos = 2*32;
    kongPlayer.height = donkeykong.height;
    kongPlayer.width = donkeykong.width;
    for (int i = 0; i< LEVELONEWIDTH; i++){
        for (int j = 0; j<LEVELONEHEIGHT; j++){
            if (levelOne[j][i] == 2){ 
            myDrawImage(pathTile.pixel_data, pathTile.width, pathTile.height, i*32,j*32, 0);        
            }
        else if (levelOne[j][i] == 1){ // if its the border
            myDrawImage(tile.pixel_data, tile.width, tile.height, i*32,j*32, 0);
        }
        else if (levelOne[j][i] == 9){
            myDrawImage(grassTile.pixel_data, grassTile.width, grassTile.height, i*32,j*32,0);  
            myDrawImage(banana.pixel_data, banana.width, banana.height, i*32,j*32,1); 
        }
         else if (levelOne[j][i] == 3){ // if its a wasp enemy
             myDrawImage(grassTile.pixel_data, grassTile.width, grassTile.height, i*32,j*32,0);  
            myDrawImage(wasp.pixel_data, wasp.width, wasp.height, i*32,j*32, 1);
        }
        else if (levelOne[j][i] == 4){ // if its the goomba
            myDrawImage(goomba.pixel_data, goomba.width, goomba.height, i*32,j*32, 1);
        }
        else if (levelOne[j][i] == 5){ // if its the other still enemy
            myDrawImage(grassTile.pixel_data, grassTile.width, grassTile.height, i*32,j*32,0); 
            myDrawImage(snakes.pixel_data, snakes.width, snakes.height, i*32,j*32, 1);
        }
        else{ // else it's just a normal tile
         myDrawImage(grassTile.pixel_data, grassTile.width, grassTile.height, i*32,j*32,0);  
        }
        }
    }
    myDrawImage(kongPlayer.image, kongPlayer.width, kongPlayer.height, kongPlayer.xpos, kongPlayer.ypos, 1);
    gameState.state = 2;
    drawLives();
    drawString(750, 170, "Time Left: ", 0xA);
    drawString(750, 300, "Score: ", 0xA);

}

void loadLevelOneFromPause(){ // to load level one from a paused state
    clearScreen();
    kongPlayer.image = donkeykong.pixel_data; // im also using the player structure so its easy to have all player data in one place
    for (int i = 0; i< LEVELONEWIDTH; i++){
        for (int j = 0; j<LEVELONEHEIGHT; j++){
            if (levelOne[j][i] == 2){ 
            myDrawImage(pathTile.pixel_data, pathTile.width, pathTile.height, i*32,j*32, 0);        
            }
        else if (levelOne[j][i] == 1){ // if its the border
            myDrawImage(tile.pixel_data, tile.width, tile.height, i*32,j*32, 0);
        }
        else if (levelOne[j][i] == 9){
            myDrawImage(grassTile.pixel_data, grassTile.width, grassTile.height, i*32,j*32,0);  
            myDrawImage(banana.pixel_data, banana.width, banana.height, i*32,j*32,1); 
        }
         else if (levelOne[j][i] == 3){ // if its a wasp enemy
             myDrawImage(grassTile.pixel_data, grassTile.width, grassTile.height, i*32,j*32,0);  
            myDrawImage(wasp.pixel_data, wasp.width, wasp.height, i*32,j*32, 1);
        }
        else if (levelOne[j][i] == 4){ // if its the goomba
            myDrawImage(goomba.pixel_data, goomba.width, goomba.height, i*32,j*32, 1);
        }
        else if (levelOne[j][i] == 5){ // if its the other still enemy
            myDrawImage(grassTile.pixel_data, grassTile.width, grassTile.height, i*32,j*32,0); 
            myDrawImage(snakes.pixel_data, snakes.width, snakes.height, i*32,j*32, 1);
        }
        else{ // else it's just a normal tile
         myDrawImage(grassTile.pixel_data, grassTile.width, grassTile.height, i*32,j*32,0);  
        }
        }
    }
    myDrawImage(kongPlayer.image, kongPlayer.width, kongPlayer.height, kongPlayer.xpos, kongPlayer.ypos, 1);
    gameState.state = 2;
    drawLives();
    drawString(750, 170, "Time Left: ", 0xA);
    drawString(750, 300, "Score: ", 0xA);

}

void loadLevelTwo(){ // to load all of level two for the first time
    clearScreen();  
    gameState.gameMap.width= LEVELTWOWIDTH;
    gameState.gameMap.height = LEVELTWOHEIGHT;
    gameState.changeLevel = 0;

    levelTwoMovingObjects[0].xpos = 7*32; // done
    levelTwoMovingObjects[0].ypos = 1*32;
    levelTwoMovingObjects[1].xpos = 5*32; // done
    levelTwoMovingObjects[1].ypos = 4*32;
    levelTwoMovingObjects[2].xpos = 9*32; // done
    levelTwoMovingObjects[2].ypos = 5*32;
    levelTwoMovingObjects[3].xpos = 6*32; // done
    levelTwoMovingObjects[3].ypos = 6*32;
    levelTwoMovingObjects[4].xpos = 16*32; // done
    levelTwoMovingObjects[4].ypos = 9*32;
    levelTwoMovingObjects[5].xpos = 16*32;  // done
    levelTwoMovingObjects[5].ypos = 12*32;
    levelTwoMovingObjects[6].xpos = 4*32;  // done
    levelTwoMovingObjects[6].ypos = 10*32;
    levelTwoMovingObjects[7].xpos = 16*32; // done
    levelTwoMovingObjects[7].ypos = 14*32;
    levelTwoMovingObjects[8].xpos = 9*32; // done
    levelTwoMovingObjects[8].ypos = 16*32;


     // ADD FOR LOOP HERE FOR SIMILAR THINGS
      for (int i = 0; i<9; i++){
        levelTwoMovingObjects[i].width = golem.width;
        levelTwoMovingObjects[i].height = golem.height;
        levelTwoMovingObjects[i].image = golem.pixel_data;
        levelTwoMovingObjects[i].move = 0;
    }
  
    gameState.gameMap.timeLeft = 120; // resetting the timer
    kongPlayer.invincible = 0; 
    kongPlayer.xpos = 1*32;
    kongPlayer.ypos = 18*32;

    for (int i = 0; i< LEVELTWOWIDTH; i++){
        for (int j = 0; j<LEVELTWOHEIGHT; j++){
            if (levelTwo[j][i] == 2){ 
            myDrawImage(pathTile2.pixel_data, pathTile2.width, pathTile2.height, i*32,j*32, 0);        
            }
        else if (levelTwo[j][i] == 1){ // if its the border
            myDrawImage(tile.pixel_data, tile.width, tile.height, i*32,j*32, 0);
        }
        else if (levelTwo[j][i] == 9){
            myDrawImage(dirtTile.pixel_data, dirtTile.width, dirtTile.height, i*32,j*32,0);  
            myDrawImage(banana.pixel_data, banana.width, banana.height, i*32,j*32,1); 
        }
         else if (levelTwo[j][i] == 3){ // if its a enemy
            myDrawImage(dirtTile.pixel_data, dirtTile.width, dirtTile.height, i*32,j*32,0);  
            myDrawImage(ant.pixel_data, ant.width, ant.height, i*32,j*32, 1);
        }
        else if (levelTwo[j][i] == 4){ // if its the moving enemy
            myDrawImage(golem.pixel_data, golem.width, golem.height, i*32,j*32, 1);
        }
        else if (levelTwo[j][i] == 5){ // if its the second still enemy
            myDrawImage(dirtTile.pixel_data, dirtTile.width, dirtTile.height, i*32,j*32,0);  
            myDrawImage(scorpion.pixel_data, scorpion.width, scorpion.height, i*32,j*32, 1);
        }
        else{ // else it's just a normal tile
         myDrawImage(dirtTile.pixel_data, dirtTile.width, dirtTile.height, i*32,j*32,0);  
        }
        }
    }
    myDrawImage(kongPlayer.image, kongPlayer.width, kongPlayer.height, kongPlayer.xpos, kongPlayer.ypos, 1);
    gameState.state = 3;
    drawLives();
    drawString(750, 170, "Time Left: ", 0xA);
    drawString(750, 300, "Score: ", 0xA);
}

void loadLevelTwoFromPause(){ // to load level two from a paused state
    clearScreen();
    for (int i = 0; i< LEVELTWOWIDTH; i++){
        for (int j = 0; j<LEVELTWOHEIGHT; j++){
            if (levelTwo[j][i] == 2){ 
            myDrawImage(pathTile2.pixel_data, pathTile2.width, pathTile2.height, i*32,j*32, 0);        
            }
        else if (levelTwo[j][i] == 1){ // if its the border
            myDrawImage(tile.pixel_data, tile.width, tile.height, i*32,j*32, 0);
        }
        else if (levelTwo[j][i] == 9){
            myDrawImage(dirtTile.pixel_data, dirtTile.width, dirtTile.height, i*32,j*32,0);  
            myDrawImage(banana.pixel_data, banana.width, banana.height, i*32,j*32,1); 
        }
         else if (levelTwo[j][i] == 3){ // if its a enemy
            myDrawImage(dirtTile.pixel_data, dirtTile.width, dirtTile.height, i*32,j*32,0);  
            myDrawImage(ant.pixel_data, ant.width, ant.height, i*32,j*32, 1);
        }
        else if (levelTwo[j][i] == 4){ // if its the moving enemy
            myDrawImage(golem.pixel_data, golem.width, golem.height, i*32,j*32, 1);
        }
        else if (levelTwo[j][i] == 5){ // if its the second still enemy
            myDrawImage(dirtTile.pixel_data, dirtTile.width, dirtTile.height, i*32,j*32,0);  
            myDrawImage(scorpion.pixel_data, scorpion.width, scorpion.height, i*32,j*32, 1);
        }
        else{ // else it's just a normal tile
         myDrawImage(dirtTile.pixel_data, dirtTile.width, dirtTile.height, i*32,j*32,0);  
        }
        }
    }
    myDrawImage(kongPlayer.image, kongPlayer.width, kongPlayer.height, kongPlayer.xpos, kongPlayer.ypos, 1);
    gameState.state = 3;
    drawLives();
    drawString(750, 170, "Time Left: ", 0xA);
    drawString(750, 300, "Score: ", 0xA);

}

void loadLevelThree(){ // to load all of level three for the first time
    clearScreen();  
    gameState.gameMap.width= LEVELTHREEWIDTH;
    gameState.gameMap.height = LEVELTHREEHEIGHT;
    gameState.changeLevel = 0;

    levelThreeMovingObjects[0].xpos = 18*32;
    levelThreeMovingObjects[0].ypos = 2*32;

    levelThreeMovingObjects[1].xpos = 2*32; 
    levelThreeMovingObjects[1].ypos = 4*32;

    levelThreeMovingObjects[2].xpos = 1*32; 
    levelThreeMovingObjects[2].ypos = 6*32;

    levelThreeMovingObjects[3].xpos = 10*32; 
    levelThreeMovingObjects[3].ypos = 5*32;

    levelThreeMovingObjects[4].xpos = 18*32; 
    levelThreeMovingObjects[4].ypos = 9*32;

    levelThreeMovingObjects[5].xpos = 13*32; 
    levelThreeMovingObjects[5].ypos = 15*32;

    levelThreeMovingObjects[6].xpos = 6*32;
    levelThreeMovingObjects[6].ypos = 18*32;

    // ADD FOR LOOP HERE FOR SIMILAR THINGS
      for (int i = 0; i<6; i++){
        levelThreeMovingObjects[i].width = iceGuard.width;
         levelThreeMovingObjects[i].height = iceGuard.height;
         levelThreeMovingObjects[i].image = iceGuard.pixel_data;
          levelThreeMovingObjects[i].move = 0;
    }


    gameState.gameMap.timeLeft = 120; // resetting the timer
    kongPlayer.invincible = 0; 
    kongPlayer.xpos = 1*32;
    kongPlayer.ypos = 1*32;

    for (int i = 0; i< LEVELTHREEWIDTH; i++){
        for (int j = 0; j<LEVELTHREEHEIGHT; j++){
            if (levelThree[j][i] == 2){ 
            myDrawImage(iceBarrier.pixel_data, iceBarrier.width, iceBarrier.height, i*32,j*32, 0);        
            }
        else if (levelThree[j][i] == 1){ // if its the border
            myDrawImage(tile.pixel_data, tile.width, tile.height, i*32,j*32, 0);
        }
        else if (levelThree[j][i] == 9){
            myDrawImage(iceTile.pixel_data, iceTile.width, iceTile.height, i*32,j*32,0);  
            myDrawImage(banana.pixel_data, banana.width, banana.height, i*32,j*32,1); 
        }
         else if (levelThree[j][i] == 3){ // if its a enemy
            myDrawImage(iceTile.pixel_data, iceTile.width, iceTile.height, i*32,j*32,0);  
            myDrawImage(iceSquid.pixel_data, iceSquid.width, iceSquid.height, i*32,j*32, 1);
        }
        else if (levelThree[j][i] == 4){ // if its the moving enemy
            myDrawImage(iceGuard.pixel_data, iceGuard.width, iceGuard.height, i*32,j*32, 1);
        }
        else if (levelThree[j][i] == 5){ // if its the other still enemy
            myDrawImage(iceTile.pixel_data, iceTile.width, iceTile.height, i*32,j*32,0);  
            myDrawImage(iceMonster.pixel_data, iceMonster.width, iceMonster.height, i*32,j*32, 1);
        }
        else{ // else it's just a normal tile
         myDrawImage(iceTile.pixel_data, iceTile.width, iceTile.height, i*32,j*32,0);  
        }
        }
    }
    myDrawImage(kongPlayer.image, kongPlayer.width, kongPlayer.height, kongPlayer.xpos, kongPlayer.ypos, 1);
    gameState.state = 4;
    drawLives();
    drawString(750, 170, "Time Left: ", 0xA);
    drawString(750, 300, "Score: ", 0xA);

}

void loadLevelThreeFromPause(){ // to load level three from a paused state
    clearScreen();
    for (int i = 0; i< LEVELTHREEWIDTH; i++){
        for (int j = 0; j<LEVELTHREEHEIGHT; j++){
            if (levelThree[j][i] == 2){ 
            myDrawImage(iceBarrier.pixel_data, iceBarrier.width, iceBarrier.height, i*32,j*32, 0);        
            }
        else if (levelThree[j][i] == 1){ // if its the border
            myDrawImage(tile.pixel_data, tile.width, tile.height, i*32,j*32, 0);
        }
        else if (levelThree[j][i] == 9){
            myDrawImage(iceTile.pixel_data, iceTile.width, iceTile.height, i*32,j*32,0);  
            myDrawImage(banana.pixel_data, banana.width, banana.height, i*32,j*32,1); 
        }
         else if (levelThree[j][i] == 3){ // if its a enemy
            myDrawImage(iceTile.pixel_data, iceTile.width, iceTile.height, i*32,j*32,0);  
            myDrawImage(iceSquid.pixel_data, iceSquid.width, iceSquid.height, i*32,j*32, 1);
        }
        else if (levelThree[j][i] == 4){ // if its the moving enemy
            myDrawImage(iceGuard.pixel_data, iceGuard.width, iceGuard.height, i*32,j*32, 1);
        }
        else if (levelThree[j][i] == 5){ // if its the other still enemy
            myDrawImage(iceTile.pixel_data, iceTile.width, iceTile.height, i*32,j*32,0);  
            myDrawImage(iceMonster.pixel_data, iceMonster.width, iceMonster.height, i*32,j*32, 1);
        }
        else{ // else it's just a normal tile
         myDrawImage(iceTile.pixel_data, iceTile.width, iceTile.height, i*32,j*32,0);  
        }
        }
    }
    myDrawImage(kongPlayer.image, kongPlayer.width, kongPlayer.height, kongPlayer.xpos, kongPlayer.ypos, 1);
    gameState.state = 4;
    drawLives();
    drawString(750, 170, "Time Left: ", 0xA);
    drawString(750, 300, "Score: ", 0xA);

}

void loadLevelFour(){ // to load all of level four for the first time
     clearScreen();  
    gameState.gameMap.width= LEVELFOURWIDTH;
    gameState.gameMap.height = LEVELFOURHEIGHT;
    gameState.changeLevel = 0;

    levelFourMovingObjects[0].xpos = 5*32; 
    levelFourMovingObjects[0].ypos = 2*32;

    levelFourMovingObjects[1].xpos = 13*32; 
    levelFourMovingObjects[1].ypos = 2*32;

    levelFourMovingObjects[2].xpos = 9*32; 
    levelFourMovingObjects[2].ypos = 7*32;


    levelFourMovingObjects[3].xpos = 13*32; 
    levelFourMovingObjects[3].ypos = 7*32;

    levelFourMovingObjects[4].xpos = 4*32; 
    levelFourMovingObjects[4].ypos = 8*32;


    levelFourMovingObjects[5].xpos = 17*32; 
    levelFourMovingObjects[5].ypos = 8*32;



    levelFourMovingObjects[6].xpos = 3*32; 
    levelFourMovingObjects[6].ypos = 10*32;




    levelFourMovingObjects[7].xpos = 11*32; 
    levelFourMovingObjects[7].ypos = 10*32;



    levelFourMovingObjects[8].xpos = 18*32; 
    levelFourMovingObjects[8].ypos = 11*32;




    levelFourMovingObjects[9].xpos = 3*32; 
    levelFourMovingObjects[9].ypos = 13*32;



    levelFourMovingObjects[10].xpos = 6*32; 
    levelFourMovingObjects[10].ypos = 13*32;




    levelFourMovingObjects[11].xpos = 10*32; 
    levelFourMovingObjects[11].ypos = 14*32;



    levelFourMovingObjects[12].xpos = 18*32; 
    levelFourMovingObjects[12].ypos = 15*32;



    levelFourMovingObjects[13].xpos = 6*32; 
    levelFourMovingObjects[13].ypos = 16*32;


    levelFourMovingObjects[14].xpos = 10*32; 
    levelFourMovingObjects[14].ypos = 16*32;



    levelFourMovingObjects[15].xpos = 14*32; 
    levelFourMovingObjects[15].ypos = 16*32;


    levelFourMovingObjects[16].xpos = 11*32; 
    levelFourMovingObjects[16].ypos = 18*32;

    

    // ADD FOR LOOP HERE FOR SIMILAR THINGS
      for (int i = 0; i<17; i++){
        levelFourMovingObjects[i].width = reaper.width;
         levelFourMovingObjects[i].height = reaper.height;
         levelFourMovingObjects[i].image = reaper.pixel_data;
          levelFourMovingObjects[i].move = 0;
    }


    gameState.gameMap.timeLeft = 120; // resetting the timer
    kongPlayer.invincible = 0; 

    kongPlayer.xpos = 18*32;
    kongPlayer.ypos = 1*32;

    for (int i = 0; i< LEVELFOURWIDTH; i++){
        for (int j = 0; j<LEVELFOURHEIGHT; j++){
            if (levelFour[j][i] == 2){ 
            myDrawImage(skullTile.pixel_data, skullTile.width, skullTile.height, i*32,j*32, 0);        
            }
        else if (levelFour[j][i] == 1){ // if its the border
            myDrawImage(tile.pixel_data, tile.width, tile.height, i*32,j*32, 0);
        }
        else if (levelFour[j][i] == 9){
            myDrawImage(fireTile.pixel_data, fireTile.width, fireTile.height, i*32,j*32,0);  
            myDrawImage(banana.pixel_data, banana.width, banana.height, i*32,j*32,1); 
        }
         else if (levelFour[j][i] == 3){ // if its a enemy (still)
            myDrawImage(fireTile.pixel_data, fireTile.width, fireTile.height, i*32,j*32,0);  
            myDrawImage(fireMonster.pixel_data, fireMonster.width, fireMonster.height, i*32,j*32, 1);
        }
        else if (levelFour[j][i] == 4){ // if its the moving enemy
            myDrawImage(reaper.pixel_data, reaper.width, reaper.height, i*32,j*32, 1);
        }
        else if (levelFour[j][i] == 5){ // if its the other still enemy
            myDrawImage(fireTile.pixel_data, fireTile.width, fireTile.height, i*32,j*32,0); 
            myDrawImage(reaper2.pixel_data, reaper2.width, reaper2.height, i*32,j*32, 1);
        }
        else{ // else it's just a normal tile
         myDrawImage(fireTile.pixel_data, fireTile.width, fireTile.height, i*32,j*32,0);  
        }
        }
    }
    myDrawImage(kongPlayer.image, kongPlayer.width, kongPlayer.height, kongPlayer.xpos, kongPlayer.ypos, 1);
    gameState.state = 5;
    drawLives();
    drawString(750, 170, "Time Left: ", 0xA);
    drawString(750, 300, "Score: ", 0xA);

}

void loadLevelFourFromPause(){ // to load level four from a paused state
    clearScreen();
     for (int i = 0; i< LEVELFOURWIDTH; i++){
        for (int j = 0; j<LEVELFOURHEIGHT; j++){
            if (levelFour[j][i] == 2){ 
            myDrawImage(skullTile.pixel_data, skullTile.width, skullTile.height, i*32,j*32, 0);        
            }
        else if (levelFour[j][i] == 1){ // if its the border
            myDrawImage(tile.pixel_data, tile.width, tile.height, i*32,j*32, 0);
        }
        else if (levelFour[j][i] == 9){
            myDrawImage(fireTile.pixel_data, fireTile.width, fireTile.height, i*32,j*32,0);  
            myDrawImage(banana.pixel_data, banana.width, banana.height, i*32,j*32,1); 
        }
         else if (levelFour[j][i] == 3){ // if its a enemy (still)
            myDrawImage(fireTile.pixel_data, fireTile.width, fireTile.height, i*32,j*32,0);  
            myDrawImage(fireMonster.pixel_data, fireMonster.width, fireMonster.height, i*32,j*32, 1);
        }
        else if (levelFour[j][i] == 4){ // if its the moving enemy
            myDrawImage(reaper.pixel_data, reaper.width, reaper.height, i*32,j*32, 1);
        }
         else if (levelFour[j][i] == 5){ // if its the other still enemy
            myDrawImage(fireTile.pixel_data, fireTile.width, fireTile.height, i*32,j*32,0); 
            myDrawImage(reaper2.pixel_data, reaper2.width, reaper2.height, i*32,j*32, 1);
        }
        else{ // else it's just a normal tile
         myDrawImage(fireTile.pixel_data, fireTile.width, fireTile.height, i*32,j*32,0);  
        }
        }
    }
    myDrawImage(kongPlayer.image, kongPlayer.width, kongPlayer.height, kongPlayer.xpos, kongPlayer.ypos, 1);
    gameState.state = 5;
    drawLives();
    drawString(750, 170, "Time Left: ", 0xA);
    drawString(750, 300, "Score: ", 0xA);
}



void moveEnemyLeft(struct Object *enemy){  // this function moves moving enemy to the left
 if (enemy->move == 0){
   int nextX = (enemy->xpos-32)/32;
    int nextY = enemy->ypos/32;
    if (gameState.state == 2){
    if (levelOne[nextY][nextX] != 1 && levelOne[nextY][nextX] != 2){
        reDrawPrevBlockEnemy(enemy->xpos, enemy->ypos);
        if (gameState.gameMap.lives > 0){ // had to check for this for all movement or else was getting weird input errors on game over screen
            enemy->xpos = nextX*32;
            myDrawImage(enemy->image, enemy->width, enemy->height, enemy->xpos, enemy->ypos, 1); 
            levelOne[nextY][nextX] = 4;
        }
    }
    }
    else if (gameState.state == 3){
    if (levelTwo[nextY][nextX] != 1 && levelTwo[nextY][nextX] != 2){
        reDrawPrevBlockEnemy(enemy->xpos, enemy->ypos);
        if (gameState.gameMap.lives > 0){ 
            enemy->xpos = nextX*32;
            myDrawImage(enemy->image, enemy->width, enemy->height, enemy->xpos, enemy->ypos, 1); 
            levelTwo[nextY][nextX] = 4;
        }
    }
    }
    else if (gameState.state == 4){
    if (levelThree[nextY][nextX] != 1 && levelThree[nextY][nextX] != 2){
        reDrawPrevBlockEnemy(enemy->xpos, enemy->ypos);
        if (gameState.gameMap.lives > 0){ 
            enemy->xpos = nextX*32;
            myDrawImage(enemy->image, enemy->width, enemy->height, enemy->xpos, enemy->ypos, 1); 
            levelThree[nextY][nextX] = 4;
        }
    }
    }
     else if (gameState.state == 5){
    if (levelFour[nextY][nextX] != 1 && levelFour[nextY][nextX] != 2 && levelFour[nextY][nextX] != 3 && levelFour[nextY][nextX] != 4 && levelFour[nextY][nextX] != 5){
        reDrawPrevBlockEnemy(enemy->xpos, enemy->ypos);
        if (gameState.gameMap.lives > 0){ // had to check for this for all movement or else was getting weird input errors on game over screen
            enemy->xpos = nextX*32;
            myDrawImage(enemy->image, enemy->width, enemy->height, enemy->xpos, enemy->ypos, 1); 
            levelFour[nextY][nextX] = 4;
        }
    }
    }
}
}


void moveEnemyRight(struct Object *enemy){ // this function moves moving enemy to the right
    if (enemy->move == 0){
    int nextX = (enemy->xpos+32)/32;
    int nextY = enemy->ypos/32;
    if (gameState.state == 2){
    if (levelOne[nextY][nextX] != 1 && levelOne[nextY][nextX] != 2){
        reDrawPrevBlockEnemy(enemy->xpos, enemy->ypos);
        if (gameState.gameMap.lives > 0){ 
        enemy->xpos = nextX*32;
        myDrawImage(enemy->image, enemy->width, enemy->height, enemy->xpos, enemy->ypos, 1); 
        levelOne[nextY][nextX] = 4; 
        }
    }
    }
    else if (gameState.state == 3){
    if (levelTwo[nextY][nextX] != 1 && levelTwo[nextY][nextX] != 2){
        reDrawPrevBlockEnemy(enemy->xpos, enemy->ypos);
        if (gameState.gameMap.lives > 0){ 
        enemy->xpos = nextX*32;
        myDrawImage(enemy->image, enemy->width, enemy->height, enemy->xpos, enemy->ypos, 1); 
        levelTwo[nextY][nextX] = 4; 
        }
    }
    }
    else if (gameState.state == 4){
    if (levelThree[nextY][nextX] != 1 && levelThree[nextY][nextX] != 2){
        reDrawPrevBlockEnemy(enemy->xpos, enemy->ypos);
        if (gameState.gameMap.lives > 0){ 
        enemy->xpos = nextX*32;
        myDrawImage(enemy->image, enemy->width, enemy->height, enemy->xpos, enemy->ypos, 1); 
        levelThree[nextY][nextX] = 4; 
        }
    }
    }

     else if (gameState.state == 5){
    if (levelFour[nextY][nextX] != 1 && levelFour[nextY][nextX] != 2 && levelFour[nextY][nextX] != 3 && levelFour[nextY][nextX] != 4 && levelFour[nextY][nextX] != 5){
        reDrawPrevBlockEnemy(enemy->xpos, enemy->ypos);
        if (gameState.gameMap.lives > 0){ 
        enemy->xpos = nextX*32;
        myDrawImage(enemy->image, enemy->width, enemy->height, enemy->xpos, enemy->ypos, 1); 
        levelFour[nextY][nextX] = 4; 
        }
    }
    }
  }
}

void moveEnemyUp(struct Object *enemy){ // this function moves moving enemy up
    if (enemy->move == 0){
    int nextX = enemy->xpos/32;
    int nextY = (enemy->ypos-32)/32;
    if (gameState.state == 4){
    if (levelThree[nextY][nextX] != 1 && levelThree[nextY][nextX] != 2){
        reDrawPrevBlockEnemy(enemy->xpos, enemy->ypos);
        if (gameState.gameMap.lives > 0){ 
        enemy->ypos = nextY*32;
        myDrawImage(enemy->image, enemy->width, enemy->height, enemy->xpos, enemy->ypos, 1); 
        levelThree[nextY][nextX] = 4; 
        }
    }
    }
    else if (gameState.state == 5){
    if (levelFour[nextY][nextX] != 1 && levelFour[nextY][nextX] != 2 && levelFour[nextY][nextX] != 3 && levelFour[nextY][nextX] != 4 && levelFour[nextY][nextX] != 5){
        reDrawPrevBlockEnemy(enemy->xpos, enemy->ypos);
        if (gameState.gameMap.lives > 0){ 
        enemy->ypos = nextY*32;
        myDrawImage(enemy->image, enemy->width, enemy->height, enemy->xpos, enemy->ypos, 1); 
        levelFour[nextY][nextX] = 4; 
        }
    }
    }

  }
}

void moveEnemyDown(struct Object *enemy){ // this function moves moving enemy down
    if (enemy->move == 0){
    int nextX = enemy->xpos/32;
    int nextY = (enemy->ypos+32)/32;
    if (gameState.state == 4){
    if (levelThree[nextY][nextX] != 1 && levelThree[nextY][nextX] != 2){
        reDrawPrevBlockEnemy(enemy->xpos, enemy->ypos);
        if (gameState.gameMap.lives > 0){ 
        enemy->ypos = nextY*32;
        myDrawImage(enemy->image, enemy->width, enemy->height, enemy->xpos, enemy->ypos, 1); 
        levelThree[nextY][nextX] = 4; 
        }
    }
    }
    else if (gameState.state == 5){
    if (levelFour[nextY][nextX] != 1 && levelFour[nextY][nextX] != 2 && levelFour[nextY][nextX] != 3 && levelFour[nextY][nextX] != 4 && levelFour[nextY][nextX] != 5){
        reDrawPrevBlockEnemy(enemy->xpos, enemy->ypos);
        if (gameState.gameMap.lives > 0){ 
        enemy->ypos = nextY*32;
        myDrawImage(enemy->image, enemy->width, enemy->height, enemy->xpos, enemy->ypos, 1); 
        levelFour[nextY][nextX] = 4; 
        }
    }
    }

  }
}


void reDrawPrevBlockEnemy(int x, int y){ // here we will replace block with whatever's been left by the moving enemy
    int xCheck = x/32;
    int yCheck = y/32;
    if (gameState.state == 2){
    myDrawImage(grassTile.pixel_data, grassTile.width, grassTile.height, x,y,0);  
    levelOne[yCheck][xCheck] = 0; // don't want tile to make you lose a life if enemy has left it, so will turn into 0 
    }
    else if (gameState.state == 3){
    myDrawImage(dirtTile.pixel_data, dirtTile.width, dirtTile.height, x,y,0);  
    levelTwo[yCheck][xCheck] = 0; // don't want tile to make you lose a life if enemy has left it, so will turn into 0 
    }
     else if (gameState.state == 4){
    myDrawImage(iceTile.pixel_data, iceTile.width, iceTile.height, x,y,0);  
    levelThree[yCheck][xCheck] = 0; // don't want tile to make you lose a life if enemy has left it, so will turn into 0 
    }
    else if (gameState.state == 5){
    myDrawImage(fireTile.pixel_data, fireTile.width, fireTile.height, x,y,0);  
    levelFour[yCheck][xCheck] = 0; // don't want tile to make you lose a life if enemy has left it, so will turn into 0 
    }

}


void reDrawPrevBlock(int x, int y){ // here we will replace block with whatever's been touched (for character). This function also checks for collisions with enemies and packs
    int xCheck = x/32;
    int yCheck = y/32;
    signal = 0;
    
    if (gameState.state == 2){
    myDrawImage(grassTile.pixel_data, grassTile.width, grassTile.height, x,y,0);  
    }
      if (gameState.state == 3){
    myDrawImage(dirtTile.pixel_data, dirtTile.width, dirtTile.height, x,y,0);  
    }

      if (gameState.state == 4){
    myDrawImage(iceTile.pixel_data, iceTile.width, iceTile.height, x,y,0);  
    }

     if (gameState.state == 5){
    myDrawImage(fireTile.pixel_data, fireTile.width, fireTile.height, x,y,0);  
    }


    // COMPARE INVINCIBILITY WITH TIME HERE
    if (gameState.gameMap.timeLeft == kongPlayer.trackInvincible-3){ // we will make donkey kong invincible to all enemies for only 3 seconds
        kongPlayer.invincible = 0;
        drawString(750, 350, "YOU ARE NO LONGER INVINCIBLE!          ", 0xA);
    }

    if (gameState.state == 2){

     if (levelOne[yCheck][xCheck] == 9){ // got to the banana
         levelOne[yCheck][xCheck] = 0; // don't want tile to make you lose another life, so will turn into 0 
        gameState.changeLevel = 1;
         gameState.gameMap.score = gameState.gameMap.score + (5 *(gameState.gameMap.timeLeft + gameState.gameMap.lives)); 
         loadLevelTwo();
     }
     if (levelOne[yCheck][xCheck] == 8){
        kongPlayer.invincible = 1;
        kongPlayer.trackInvincible = gameState.gameMap.timeLeft;
        drawString(750, 350, "YOU ARE INVINCIBLE FOR 3 SECONDS!", 0xA);
         levelOne[yCheck][xCheck] = 0; // don't want tile to make you lose another life, so will turn into 0 

     }


     if (levelOne[yCheck][xCheck] == 3 || levelOne[yCheck][xCheck] == 4 || levelOne[yCheck][xCheck] == 5){
        if (levelOne[yCheck][xCheck] == 4){
            signal = 1;
         }
          if (signal == 1){ 
            for (int i = 0; i< 3; i++){
                if (levelOneMovingObjects[i].xpos/32 == xCheck && levelOneMovingObjects[i].ypos/32 == yCheck){
                 levelOneMovingObjects[i].move = 1;
            }
        }

          }
        if (kongPlayer.invincible != 1){
         gameState.gameMap.lives = gameState.gameMap.lives - 1;
         loseLive();
        }
         levelOne[yCheck][xCheck] = 0; // don't want tile to make you lose another life, so will turn into 0 
     }
      if (levelOne[yCheck][xCheck]==6 && gameState.gameMap.timeLeft <= 120-30){ // gain a life
            gameState.gameMap.lives = gameState.gameMap.lives + 1;
            drawString(750, 350, "YOU GAINED A LIVE!               ", 0xA);
            levelOne[yCheck][xCheck] = 0;
            drawLives();
        }

       if (levelOne[yCheck][xCheck]==7 && gameState.gameMap.timeLeft <= 120-30){ // gain time
            gameState.gameMap.timeLeft =  gameState.gameMap.timeLeft + 5;
            drawString(750, 350, "YOU GAINED 5 SECONDS!             ", 0xA);
            levelOne[yCheck][xCheck] = 0;
        } 
     if (gameState.gameMap.lives<= 0){
        gameState.loseCondition = 1;
        clearScreen();
        gameOverRETRY();
     }
}

    else if (gameState.state == 3){

     if (levelTwo[yCheck][xCheck] == 9){ // got to the banana
         levelTwo[yCheck][xCheck] = 0; // don't want tile to make you lose another life, so will turn into 0 
        gameState.changeLevel = 1;
        gameState.gameMap.score = gameState.gameMap.score + (5 *(gameState.gameMap.timeLeft + gameState.gameMap.lives)); 
        loadLevelThree(); 
     }
     if (levelTwo[yCheck][xCheck] == 8){
        kongPlayer.invincible = 1;
        kongPlayer.trackInvincible = gameState.gameMap.timeLeft;
         drawString(750, 350, "YOU ARE INVINCIBLE FOR 3 SECONDS!", 0xA);
         levelTwo[yCheck][xCheck] = 0; // don't want tile to make you lose another life, so will turn into 0 
     }


     if (levelTwo[yCheck][xCheck] == 3 || levelTwo[yCheck][xCheck] == 4 || levelTwo[yCheck][xCheck] == 5){
        if (levelTwo[yCheck][xCheck] == 4){
            signal = 1;
         }
          if (signal == 1){ 
            for (int i = 0; i< 3; i++){
                if (levelTwoMovingObjects[i].xpos/32 == xCheck && levelTwoMovingObjects[i].ypos/32 == yCheck){
                 levelTwoMovingObjects[i].move = 1;
            }
        }

          }
        if (kongPlayer.invincible != 1){
         gameState.gameMap.lives = gameState.gameMap.lives - 1;
         loseLive();
        }
         levelTwo[yCheck][xCheck] = 0; // don't want tile to make you lose another life, so will turn into 0 
     }
      if (levelTwo[yCheck][xCheck]==6 && gameState.gameMap.timeLeft <= 120-30){ // gain a life
            gameState.gameMap.lives = gameState.gameMap.lives + 1;
            drawString(750, 350, "YOU GAINED A LIVE!               ", 0xA);
            levelTwo[yCheck][xCheck] = 0;
            drawLives();
        }

       if (levelTwo[yCheck][xCheck]==7 && gameState.gameMap.timeLeft <= 120-30){ // gain time
            gameState.gameMap.timeLeft =  gameState.gameMap.timeLeft + 5;
            drawString(750, 350, "YOU GAINED 5 SECONDS!             ", 0xA);
            levelTwo[yCheck][xCheck] = 0;
        } 
     if (gameState.gameMap.lives<= 0){
        gameState.loseCondition = 1;
        clearScreen();
        gameOverRETRY();
     }
}

else if (gameState.state == 4){

     if (levelThree[yCheck][xCheck] == 9){ // got to the banana
         levelThree[yCheck][xCheck] = 0; // don't want tile to make you lose another life, so will turn into 0 
        gameState.changeLevel = 1;
        gameState.gameMap.score = gameState.gameMap.score + (5 *(gameState.gameMap.timeLeft + gameState.gameMap.lives)); 
         loadLevelFour(); 
     }
     if (levelThree[yCheck][xCheck] == 8){
        kongPlayer.invincible = 1;
        kongPlayer.trackInvincible = gameState.gameMap.timeLeft;
         drawString(750, 350, "YOU ARE INVINCIBLE FOR 3 SECONDS!", 0xA);
         levelThree[yCheck][xCheck] = 0; // don't want tile to make you lose another life, so will turn into 0 
     }


     if (levelThree[yCheck][xCheck] == 3 || levelThree[yCheck][xCheck] == 4|| levelThree[yCheck][xCheck] == 5){
        if (levelThree[yCheck][xCheck] == 4){
            signal = 1;
         }
          if (signal == 1){ 
            for (int i = 0; i<17; i++){
                if (levelThreeMovingObjects[i].xpos/32 == xCheck && levelThreeMovingObjects[i].ypos/32 == yCheck){
                 levelThreeMovingObjects[i].move = 1;
            }
        }

          }
        if (kongPlayer.invincible != 1){
         gameState.gameMap.lives = gameState.gameMap.lives - 1;
         loseLive();
        }
         levelThree[yCheck][xCheck] = 0; // don't want tile to make you lose another life, so will turn into 0 
     }
      if (levelThree[yCheck][xCheck]==6 && gameState.gameMap.timeLeft <= 120-30){ // gain a life
            gameState.gameMap.lives = gameState.gameMap.lives + 1;
            drawString(750, 350, "YOU GAINED A LIVE!               ", 0xA);
            levelThree[yCheck][xCheck] = 0;
            drawLives();
        }

       if (levelThree[yCheck][xCheck]==7 && gameState.gameMap.timeLeft <= 120-30){ // gain time
            gameState.gameMap.timeLeft =  gameState.gameMap.timeLeft + 5;
            drawString(750, 350, "YOU GAINED 5 SECONDS!             ", 0xA);
            levelThree[yCheck][xCheck] = 0;
        } 
     if (gameState.gameMap.lives<= 0){
        gameState.loseCondition = 1;
        clearScreen();
        gameOverRETRY();
     }
}

else if (gameState.state == 5){

     if (levelFour[yCheck][xCheck] == 9){ // got to the banana
         levelFour[yCheck][xCheck] = 0; // don't want tile to make you lose another life, so will turn into 0 
        gameState.changeLevel = 1;
        gameState.gameMap.score = gameState.gameMap.score + (5 *(gameState.gameMap.timeLeft + gameState.gameMap.lives)); 
        gameState.winCondition = 1;
        gameWonPLAY(); 
     }
     if (levelFour[yCheck][xCheck] == 8){
        kongPlayer.invincible = 1;
        kongPlayer.trackInvincible = gameState.gameMap.timeLeft;
         drawString(750, 350, "YOU ARE INVINCIBLE FOR 3 SECONDS!", 0xA);
         levelFour[yCheck][xCheck] = 0; // don't want tile to make you lose another life, so will turn into 0 
     }


     if (levelFour[yCheck][xCheck] == 3 || levelFour[yCheck][xCheck] == 4|| levelFour[yCheck][xCheck] == 5){
        if (levelFour[yCheck][xCheck] == 4){
            signal = 1;
         }
          if (signal == 1){ 
            for (int i = 0; i<7; i++){
                if (levelFourMovingObjects[i].xpos/32 == xCheck && levelFourMovingObjects[i].ypos/32 == yCheck){
                 levelFourMovingObjects[i].move = 1;
            }
        }

          }
        if (kongPlayer.invincible != 1){
         gameState.gameMap.lives = gameState.gameMap.lives - 1;
         loseLive();
        }
         levelFour[yCheck][xCheck] = 0; // don't want tile to make you lose another life, so will turn into 0 
     }
      if (levelFour[yCheck][xCheck]==6 && gameState.gameMap.timeLeft <= 120-30){ // gain a life
            gameState.gameMap.lives = gameState.gameMap.lives + 1;
            drawString(750, 350, "YOU GAINED A LIVE!               ", 0xA);
            levelFour[yCheck][xCheck] = 0;
            drawLives();
        }

       if (levelFour[yCheck][xCheck]==7 && gameState.gameMap.timeLeft <= 120-30){ // gain time
            gameState.gameMap.timeLeft =  gameState.gameMap.timeLeft + 5;
            drawString(750, 350, "YOU GAINED 5 SECONDS!             ", 0xA);
            levelFour[yCheck][xCheck] = 0;
        } 
     if (gameState.gameMap.lives<= 0){
        gameState.loseCondition = 1;
        clearScreen();
        gameOverRETRY();
     }
}


}



void moveLeft(){  // this function moves the character to the left
    int nextX = (kongPlayer.xpos-32)/32;
    int nextY = kongPlayer.ypos/32;
    if (gameState.state == 2){
    if (levelOne[nextY][nextX] != 1 && levelOne[nextY][nextX] != 2){
        reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
        if (gameState.gameMap.lives > 0){ // had to check for this for all movement or else was getting weird input errors on game over screen
            kongPlayer.xpos = nextX*32;
            reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos); // new POSITION
            if (gameState.changeLevel != 1){
            myDrawImage(kongPlayer.image, kongPlayer.width, kongPlayer.height, kongPlayer.xpos, kongPlayer.ypos, 1); 
        }
    }
    }
    }

    else if (gameState.state == 3){
    if (levelTwo[nextY][nextX] != 1 && levelTwo[nextY][nextX] != 2){
        reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
        if (gameState.gameMap.lives > 0){ // had to check for this for all movement or else was getting weird input errors on game over screen
            kongPlayer.xpos = nextX*32;
            reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
             if (gameState.changeLevel != 1){
            myDrawImage(kongPlayer.image, kongPlayer.width, kongPlayer.height, kongPlayer.xpos, kongPlayer.ypos, 1); 
        }
        }
    }
    }

    else if (gameState.state == 4){
    if (levelThree[nextY][nextX] != 1 && levelThree[nextY][nextX] != 2){
        reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
        if (gameState.gameMap.lives > 0){ // had to check for this for all movement or else was getting weird input errors on game over screen
            kongPlayer.xpos = nextX*32;
            reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
             if (gameState.changeLevel != 1){
            myDrawImage(kongPlayer.image, kongPlayer.width, kongPlayer.height, kongPlayer.xpos, kongPlayer.ypos, 1); 
        }
        }
    }
    }
      else if (gameState.state == 5){
    if (levelFour[nextY][nextX] != 1 && levelFour[nextY][nextX] != 2){
        reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
        if (gameState.gameMap.lives > 0){ // had to check for this for all movement or else was getting weird input errors on game over screen
            kongPlayer.xpos = nextX*32;
            reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
             if (gameState.changeLevel != 1){
            myDrawImage(kongPlayer.image, kongPlayer.width, kongPlayer.height, kongPlayer.xpos, kongPlayer.ypos, 1); 
        }
        }
    }
    }

}


void moveRight(){ // this function moves the character to the right 
    int nextX = (kongPlayer.xpos+32)/32;
    int nextY = kongPlayer.ypos/32;
    if (gameState.state == 2){
    if (levelOne[nextY][nextX] != 1 && levelOne[nextY][nextX] != 2){
        reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
        if (gameState.gameMap.lives > 0){ 
        kongPlayer.xpos = nextX*32;
        reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
        if (gameState.changeLevel != 1){
        myDrawImage(kongPlayer.image, kongPlayer.width, kongPlayer.height, kongPlayer.xpos, kongPlayer.ypos, 1); 
        }
        }
    }
    }
    else if (gameState.state == 3){
    if (levelTwo[nextY][nextX] != 1 && levelTwo[nextY][nextX] != 2){
        reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
        if (gameState.gameMap.lives > 0){ 
        kongPlayer.xpos = nextX*32;
        reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
        if (gameState.changeLevel != 1){
        myDrawImage(kongPlayer.image, kongPlayer.width, kongPlayer.height, kongPlayer.xpos, kongPlayer.ypos, 1); 
        }
        }
    }
    }

    else if (gameState.state == 4){
    if (levelThree[nextY][nextX] != 1 && levelThree[nextY][nextX] != 2){
        reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
        if (gameState.gameMap.lives > 0){ 
        kongPlayer.xpos = nextX*32;
        reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
        if (gameState.changeLevel != 1){
        myDrawImage(kongPlayer.image, kongPlayer.width, kongPlayer.height, kongPlayer.xpos, kongPlayer.ypos, 1); 
        }
        }
    }
    }
    else if (gameState.state == 5){
    if (levelFour[nextY][nextX] != 1 && levelFour[nextY][nextX] != 2){
        reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
        if (gameState.gameMap.lives > 0){ 
        kongPlayer.xpos = nextX*32;
        reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
        if (gameState.changeLevel != 1){
        myDrawImage(kongPlayer.image, kongPlayer.width, kongPlayer.height, kongPlayer.xpos, kongPlayer.ypos, 1); 
        }
        }
    }
    }

}

void moveUp(){  // this function moves character up
    int nextX = kongPlayer.xpos/32;
    int nextY = (kongPlayer.ypos-32)/32;
    if (gameState.state == 2){
    if (levelOne[nextY][nextX] != 1 && levelOne[nextY][nextX] != 2){
        reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
        if (gameState.gameMap.lives > 0){
        kongPlayer.ypos = nextY*32;
        reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
        if (gameState.changeLevel != 1){
        myDrawImage(kongPlayer.image, kongPlayer.width, kongPlayer.height, kongPlayer.xpos, kongPlayer.ypos, 1);   
        }     
    }
    }
    }
    else if (gameState.state == 3){
    if (levelTwo[nextY][nextX] != 1 && levelTwo[nextY][nextX] != 2){
        reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
         if (gameState.gameMap.lives > 0 && gameState.changeLevel != 1){
        kongPlayer.ypos = nextY*32;
        reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
         if (gameState.changeLevel != 1){
        myDrawImage(kongPlayer.image, kongPlayer.width, kongPlayer.height, kongPlayer.xpos, kongPlayer.ypos, 1);  
         }      
    }
    }
    }

    else if (gameState.state == 4){
    if (levelThree[nextY][nextX] != 1 && levelThree[nextY][nextX] != 2){
        reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
         if (gameState.gameMap.lives > 0 && gameState.changeLevel != 1){
        kongPlayer.ypos = nextY*32;
        reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
         if (gameState.changeLevel != 1){
        myDrawImage(kongPlayer.image, kongPlayer.width, kongPlayer.height, kongPlayer.xpos, kongPlayer.ypos, 1);  
         }      
    }
    }
    }

     else if (gameState.state == 5){
    if (levelFour[nextY][nextX] != 1 && levelFour[nextY][nextX] != 2){
        reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
         if (gameState.gameMap.lives > 0 && gameState.changeLevel != 1){
        kongPlayer.ypos = nextY*32;
        reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
         if (gameState.changeLevel != 1){
        myDrawImage(kongPlayer.image, kongPlayer.width, kongPlayer.height, kongPlayer.xpos, kongPlayer.ypos, 1);  
         }      
    }
    }
    }

}

void moveDown(){  // this function moves character down
    int nextX = kongPlayer.xpos/32;
    int nextY = (kongPlayer.ypos+32)/32;
    if (gameState.state == 2){
    if (levelOne[nextY][nextX] != 1 && levelOne[nextY][nextX] != 2){
         reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
        if (gameState.gameMap.lives > 0 && gameState.changeLevel != 1){
        kongPlayer.ypos = nextY*32;
         reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
         if (gameState.changeLevel != 1){
        myDrawImage(kongPlayer.image, kongPlayer.width, kongPlayer.height, kongPlayer.xpos, kongPlayer.ypos, 1); 
         }
    }
    }
    }
    else if (gameState.state == 3){
    if (levelTwo[nextY][nextX] != 1 && levelTwo[nextY][nextX] != 2){
         reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
        if (gameState.gameMap.lives > 0 && gameState.changeLevel != 1){
        kongPlayer.ypos = nextY*32;
         reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
         if (gameState.changeLevel != 1){
        myDrawImage(kongPlayer.image, kongPlayer.width, kongPlayer.height, kongPlayer.xpos, kongPlayer.ypos, 1); 
         }
    }
    }
    }
     else if (gameState.state == 4){
    if (levelThree[nextY][nextX] != 1 && levelThree[nextY][nextX] != 2){
         reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
        if (gameState.gameMap.lives > 0 && gameState.changeLevel != 1){
        kongPlayer.ypos = nextY*32;
         reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
         if (gameState.changeLevel != 1){
        myDrawImage(kongPlayer.image, kongPlayer.width, kongPlayer.height, kongPlayer.xpos, kongPlayer.ypos, 1); 
         }
    }
    }
    }
     else if (gameState.state == 5){
    if (levelFour[nextY][nextX] != 1 && levelFour[nextY][nextX] != 2){
         reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
        if (gameState.gameMap.lives > 0 && gameState.changeLevel != 1){
        kongPlayer.ypos = nextY*32;
         reDrawPrevBlock(kongPlayer.xpos, kongPlayer.ypos);
         if (gameState.changeLevel != 1){
        myDrawImage(kongPlayer.image, kongPlayer.width, kongPlayer.height, kongPlayer.xpos, kongPlayer.ypos, 1); 
         }
    }
    }
    }

}

void pauseGameRestart(){  // this function loads the pause game screen with cursor on "restart"
    if (gameState.state != 10 && gameState.state != 11){
    prevGamestate = gameState.state;
    }
    myDrawImage(pausedRestart.pixel_data,pausedRestart.width, pausedRestart.height,0,0, 0);
    gameState.state = 10;
}

void pauseGameQuit(){ // this function loads the pause game screen with cursor on "quit"
    if (gameState.state != 10 && gameState.state != 11){
    prevGamestate = gameState.state;
    }
    myDrawImage(pausedQuit.pixel_data, pausedQuit.width, pausedQuit.height,0,0, 0);
    gameState.state = 11;
}

int main()

{   
    int checkMillisecondsTimer = 0; // to continously check milliseconds counted
    char timerBuffer[4]; // to print out time 
    char scoreBuffer[10]; // to print out score
    int timerX = 850;   // x coordinate of timer on screen
    int timerY = 170;   // y coordinate of timer on screen

    gameState.state = 0; // to tell us what gameState.state we should be on and all of its attributes VERY IMPORTANT!!

                    // 0= main menu (hover over START), 1= main menu (hover over END) 2= level 1, 3= level 2, 4= level 3, 5= level 4 
                    // 6 = you win play again 7 = you win quit 8 = game over replay 9 = game over quit, 10 = paused game RESTART, 11= paused game quit
                    // gameState controls everything.

    // Initialize GPIO lines for SNES controller (1 = input, 0 = output)
    Init_GPIO(CLK, 1);
    Init_GPIO(CLK ,0);
    Init_GPIO(LAT, 1);
    Init_GPIO(LAT, 0);
    Init_GPIO(DAT, 1);

    fb_init();

       // to prevent game from potentially starting early through misclick
      int *preventEarlyStart = Read_SNES();
        for (int i = 0; i<16; i++){
            if (preventEarlyStart[i]!=1){
            while (preventEarlyStart[i]!= 1){
                Read_SNES();
            }
            }
        }

    drawString(640, 360, "Starting game....", 0xB); 

     

    int *checkPressStart = Read_SNES();
        for (int i = 0; i<16; i++){
            if (checkPressStart[i]==1){
                clearScreen();
                drawMainMenuSTART();
                break;
            }
        }
    


    while (1)

    {
     
     
        // to prevent him from going too fast, I had to make movement once per button click
        int *checkPressed = Read_SNES();
        for (int i = 0; i<16; i++){
            if (checkPressed[i]!=1){
            while (checkPressed[i]!= 1){
                Read_SNES();
            }
            }
        }

  
        // Reading user button input(there are simple if-else if statements for every possible button)
        // to tell us what gameState.state we should be on and all of its attributes. AGAIN:
                    // 0= main menu (hover over START), 1= main menu (hover over END) 2= level 1, 3= level 2, 4= level 3, 5= level 4 
                    // 6 = you win play again 7 = you win quit 8 = game over replay 9 = game over quit, 10 = pause game RESTART, 11= pause game quit
        while (1)

        {
             if (gameState.state >= 2 && gameState.state <=5){
                Wait(1000); // this is waiting 1 millisecond. 1000 microseconds = 1 millisecond. 1000 milliseconds = 1 second
                checkMillisecondsTimer = checkMillisecondsTimer + 1;  // keeping track of milliseconds (initialized as 0 near main)
            if (checkMillisecondsTimer == 1000){ // if 1000 milliseconds have been counted, then thats 1 second
                // clear the old timer value
                for (int i = timerX; i < timerX+50; i++){
                for (int j=timerY; j<timerY+25; j++){
                 drawPixel(i, j, 0xFF000000);
                }
                }

                // initialized time left as 120 seconds in every level 

                gameState.gameMap.timeLeft =  gameState.gameMap.timeLeft - 1; // lose 1 second in the game

                // HERE, CHOOSE A RANDOM NUMBER for movement (depending on level movement is different)
                if (gameState.state == 2){
                for (int i = 0; i<3; i++){
                int randomNum1 = randomMove(gameState.gameMap.timeLeft);
                if (levelOneMovingObjects[i].move == 0){
                if (randomNum1 == 0){
                moveEnemyRight(&levelOneMovingObjects[i]);
                }
                else{
                    moveEnemyLeft(&levelOneMovingObjects[i]);
                }
                }
            }
            }

            else if (gameState.state == 3){
                for (int i = 0; i<9; i++){
                if (levelTwoMovingObjects[i].move == 0){
                int randomNum2 = randomMove(gameState.gameMap.timeLeft);
                if (randomNum2 == 0){
                moveEnemyRight(&levelTwoMovingObjects[i]);
                }
                else{
                    moveEnemyLeft(&levelTwoMovingObjects[i]);
                }
                }
            }
            }
               else if (gameState.state == 4){
                for (int i = 0; i<6; i++){
                if (levelThreeMovingObjects[i].move == 0){
                int randomNum3 = randomMove(gameState.gameMap.timeLeft);
                if (randomNum3 == 0){
                moveEnemyUp(&levelThreeMovingObjects[i]);
                }
                else{
                    moveEnemyDown(&levelThreeMovingObjects[i]);
                }
                }
            }
            }

            else if (gameState.state == 5){  // GOTTA DEAL W DIFFERENTLY THIS TIME, WANT ENEMY TO MOVE ALL 4 DIRECTIONS
                for (int i = 0; i<17; i++){
                if (levelFourMovingObjects[i].move == 0){
                int randomNum4 = randomMove(gameState.gameMap.timeLeft);
                if (randomNum4 == 1){
                moveEnemyUp(&levelFourMovingObjects[i]);
                }
                else if (randomNum4 == 2){
                    moveEnemyLeft(&levelFourMovingObjects[i]);
                }
                else if (randomNum4 == 3){
                    moveEnemyRight(&levelFourMovingObjects[i]);
                }
                else{
                    moveEnemyDown(&levelFourMovingObjects[i]);
                }
                }

                }
            }
                
                
            if (gameState.gameMap.timeLeft <= 0){ // if you've run out of time, just print game over in the timer section instead of any numbers
                drawString(timerX, timerY, "Game Over", 0xA);
                gameState.loseCondition = 1;
                gameOverRETRY();  
                gameState.gameMap.timeLeft = 120; // attempt to fix problem where restart of game after time has expired was weird (still happens, hardware problem?)
            }
            else{ // else, turn the timeLeft into a string, and display the new time, with 1 second less
            intToStr(gameState.gameMap.timeLeft, timerBuffer); // converting the timeLeft into a string to print
            drawString(timerX, timerY, timerBuffer, 0xA); // drawing the new time left on the screen
            intToStr(gameState.gameMap.score, scoreBuffer);
            drawString(800, 300, scoreBuffer, 0xA);
            if (gameState.gameMap.timeLeft == 90){ // 30 seconds into the game, can draw packs
            drawPacks();
            drawString(750, 350, "Power ups have spawned!             ", 0xA);
            }

            checkMillisecondsTimer = 0; // initializing milliseconds counted back to 0 for the next second 

            }
            }
            }
           
            int* buttonsPressed = Read_SNES(); // calling Read_SNES(), saving it's array in buttonsPressed so we can look at its values

 
            if (buttonsPressed[SNES_B-1] == 0){
                

            break;

            }

            else if (buttonsPressed[SNES_Y-1] == 0){


            break;

            }

            else if (buttonsPressed[SNES_SELECT-1] == 0){

                    break;

                }

            else if (buttonsPressed[SNES_START-1] == 0){
                    if (gameState.state >= 2 && gameState.state <=5) { //must make it into pause menu later!
                        pauseGameRestart();
                        break;
                    }

                    if (gameState.state == 10){  

                    if (prevGamestate == 2){
                            loadLevelOneFromPause();
                    }
                    else if (prevGamestate == 3){
                        loadLevelTwoFromPause();
                    }
                    else if (prevGamestate == 4){
                        loadLevelThreeFromPause();
                    }
                    else if (prevGamestate == 5){
                        loadLevelFourFromPause();
                    }

                    }
                    break;

                }

                else if (buttonsPressed[SNES_UP-1]== 0){

                    if (gameState.state == 1){ // its pointed at QUIT in the main menu
                        drawMainMenuSTART();
                    }
                      if (gameState.state >= 2 && gameState.state <=5 && gameState.changeLevel != 1){ // if we're in the first level, we want the up button to execute the moveup function
                        moveUp();  
                    }

                    if (gameState.state == 7){
                        gameWonPLAY();
                    }

                    if (gameState.state == 9){ // at the game over menu where we're hovering over quit 
                        gameOverRETRY();
                    }

                    if (gameState.state == 11){
                        pauseGameRestart();
                    }



                    break;

                }

                else if (buttonsPressed[SNES_DOWN-1] == 0){
                    if (gameState.state == 0){ // its pointed at START
                        drawMainMenuQUIT(); 
                    }
                      if (gameState.state >= 2 && gameState.state <=5 && gameState.changeLevel != 1){ // if we're in the first level, we want the down button to execute the movedown function
                        moveDown(); 
                    }
                    if (gameState.state == 6){
                        gameWonQUIT();
                    }
                      if (gameState.state == 8){ // at the game over menu where we're hovering over retry
                        gameOverQUIT();
                    }

                    if (gameState.state == 10){
                        pauseGameQuit();
                    }


                    break;

                }

                else if (buttonsPressed[SNES_LEFT-1]== 0){
                     if (gameState.state >= 2 && gameState.state <=5 && gameState.changeLevel != 1){ // if we're in the first level, we want the left button to execute the moveleft function
                        moveLeft();
                    }

                    break;

                }

                else if (buttonsPressed[SNES_RIGHT-1] == 0){
                    if (gameState.state >= 2 && gameState.state <=5 && gameState.changeLevel != 1){ // if we're in the first level, we want the right button to execute the moveright function
                        moveRight();
                    }


                    break;

                }

                else if (buttonsPressed[SNES_A-1] == 0){
                    if (gameState.state == 0 || gameState.state == 6 || gameState.state == 8||gameState.state == 10){ // we want to start the game all over again for first the first time
                        resetLevels();
                        loadLevelOne();
                    }
                    if (gameState.state == 1){ // we want to close the game
                        clearScreen();
                        return 0;
                    }
                  
                      if (gameState.state == 7 || gameState.state == 9 || gameState.state == 11){ // we want to go back to the main menu
                        clearScreen();
                        drawMainMenuSTART(); 
                    }


                    break;

                }
                // these buttons dont have any functions yet, can code those later if needed
                else if (buttonsPressed[SNES_X-1] == 0){


                    break;

                }

                else if (buttonsPressed[SNES_TLEFT-1] == 0){


                    break;

                }

                else if (buttonsPressed[SNES_TRIGHT-1] == 0){


                    break;

                }


                }

                }

 return 0;

}


