#include <SPI.h>        //SPI.h must be included as DMD is written by SPI (the IDE complains otherwise)
#include <DMD.h>        //
#include <TimerOne.h>   //
#include "SystemFont5x7.h"
#include "Arial14.h"

//Fire up the DMD library as dmd
#define DISPLAYS_ACROSS 1
#define DISPLAYS_DOWN 1
DMD dmd(DISPLAYS_ACROSS, DISPLAYS_DOWN);



const int player1pin = 2;     // the number of the pushbutton pin
const int player2pin = 3;     // the number of the pushbutton pin
const int ledPin =  13;       // the number of the LED pin
const int DEBOUNCE_DELAY = 50; // 50ms
const int NUMBER_OF_PLAYERS = 2;

const int TRUE = 1;
const int FALSE = 0;


const int GAME_WAITING = 0;
const int GAME_START = 1;
const int GAME_ON = 2;
const int GAME_OVER = 3;

int game_state = GAME_START;

// States of the button
int UNPRESSED = 1;
int PRESSED = 2;

typedef struct Player {
  int pin;
  int buttonInputState;
  int buttonState;
  int score;
  unsigned long lastDebounceTime;  // the last time the output pin was toggled
  unsigned long debounceDelay;    // the debounce time; increase if the output flickers
} Player;

Player player1;
Player player2;

// Allocate memory for two Player structs.
Player players[2];

// Interrupt handler for Timer1 (TimerOne) driven DMD refresh scanning, this gets
// called at the period set in Timer1.initialize();
void ScanDMD() { 
  dmd.scanDisplayBySPI();
}


void setup() {
  Serial.begin(9600);

 
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(player1pin, INPUT);
  pinMode(player2pin, INPUT);

  for (int i = 0; i < NUMBER_OF_PLAYERS; i++) {
    players[i].buttonInputState = LOW;
    players[i].buttonState = UNPRESSED;
    players[i].score = 0;
    players[i].lastDebounceTime = 0;  // the last time the output pin was toggled
  }
  
  players[0].pin = player1pin;
  players[1].pin = player2pin;

//initialize TimerOne's interrupt/CPU usage used to scan and refresh the display
   Timer1.initialize( 1000 );           //period in microseconds to call ScanDMD. Anything longer than 5000 (5ms) and you can see flicker.
   Timer1.attachInterrupt( ScanDMD );   //attach the Timer1 interrupt to ScanDMD which goes to dmd.scanDisplayBySPI()

   //clear/init the DMD pixels held in RAM
   dmd.clearScreen( true );   //true is normal (all pixels off), false is negative (all pixels on)
}

int wait_for_players() {
  // show something on the screen to tell players to both hit their buttons
  
  dmd.selectFont(SystemFont5x7);

  char *message = "Press both buttons to start";
  
  dmd.drawMarquee(message,strlen(message),(32*DISPLAYS_ACROSS)-1,5);
  long start=millis();
  long timer=start;
  boolean ret=false;
  while(!ret){
    if ((timer+40) < millis()) {
      ret=dmd.stepMarquee(-1,0);
      timer=millis();
    }

    if (digitalRead(player1pin) == HIGH && digitalRead(player2pin) == HIGH) {
      break;    
    }
  }

  return GAME_START;
}

int tell_players_its_on_like_donkey_kong() {
  dmd.clearScreen( true );
  dmd.selectFont(SystemFont5x7);
  for (int i = 0; i < 4; i++) {
    dmd.clearScreen( true );
    delay(100);
    dmd.drawString(2, 5, "Start", 5, GRAPHICS_NORMAL );
    delay(200);
  }
  delay(1000);
  return GAME_ON;
}

int there_is_a_winner() {
  int p1 = players[0].score;
  int p2 = players[1].score;

  int score_difference = p1 - p2;
  return ((p1 >= 21 || p2 >= 21) && (score_difference >= 2 || score_difference <= -2));
}

int game_on() {
  update_score(players+0);
  update_score(players+1);
  
  display_score();

  if (there_is_a_winner()) {
    return GAME_OVER;
  } else {
    return GAME_ON;
  }
}

void update_score(Player *player) {
  //  Read the button's state
  int currentButtonState = digitalRead(player->pin);

  if (player->buttonInputState != currentButtonState) {
    player->lastDebounceTime = millis();
    
  }

  player->buttonInputState = currentButtonState;
  
  if ((millis() - player->lastDebounceTime) > DEBOUNCE_DELAY) {
    if (player->buttonInputState == HIGH) {
      if (player->buttonState == UNPRESSED) {
        player->buttonState = PRESSED;
        player->score += 1;
        Serial.println(player->score);
      }
    }
  
    if (player->buttonInputState == LOW) {
  
      if (player->buttonState == PRESSED) {
        player->buttonState = UNPRESSED;
      }
    }
  }
}

void display_score() {
}

int game_over() {
  // Show winner
  Serial.println("GAME OVER");
  players[0].score = 0;
  players[1].score = 0;
  delay(5000);
  return GAME_WAITING;
}

void loop() {
  switch(game_state) {
    case GAME_WAITING :
      game_state = wait_for_players();
      break;
    case GAME_START :
      game_state = tell_players_its_on_like_donkey_kong();
      break;
    case GAME_ON :
      game_state = game_on();
      break;
    case GAME_OVER :
      game_state = game_over();
      break;
  }  
}
