const int player1pin = 2;     // the number of the pushbutton pin
const int player2pin = 3;     // the number of the pushbutton pin
const int ledPin =  13;       // the number of the LED pin
const int DEBOUNCE_DELAY = 50; // 50ms
const int NUMBER_OF_PLAYERS = 2;

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

void loop() {
  for (int i = 0; i < NUMBER_OF_PLAYERS; i++) {
    updateButtonState(players+i);
    drawScore(players+i);
  }
}

void updateButtonState(Player *player) {
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

void drawScore(Player *player) {
}

