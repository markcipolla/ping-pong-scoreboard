const int player1pin = 2;     // the number of the pushbutton pin
const int player2pin = 3;     // the number of the pushbutton pin
const int ledPin =  13;       // the number of the LED pin
const int DEBOUNCE_DELAY = 50; // 50ms
const int NUMBER_OF_PLAYERS = 2;

const int TRUE = 1;
const int FALSE = 0;


const int GAME_WAITING = 0;
const int GAME_ON = 1;
const int GAME_OVER = 2;

int game_state = GAME_WAITING;

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
}



int wait_for_players() {
  // show something on the screen to tell players to both hit their buttons
  Serial.println("PRESS BOTH BUTTONS TO START");
  if (digitalRead(player1pin) == HIGH && digitalRead(player2pin) == HIGH) {
    tell_players_its_on_like_donkey_kong();
    return GAME_ON;
  } else {
    return GAME_WAITING;
  }
}

void tell_players_its_on_like_donkey_kong() {
  // TELL THE SCREEN IT'S BEGUN
  Serial.println("GAME ON!");
  delay(5000);
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
  delay(5000);
  return GAME_WAITING;
}

void loop() {
  switch(game_state) {
    case GAME_WAITING :
      game_state = wait_for_players();
      break;
    case GAME_ON :
      game_state = game_on();
      break;
    case GAME_OVER :
      game_state = game_over();
      break;
  }  
}
