// ************************************************************************
//  Author: Brendon McBain
//  Date: 01/07/2016
//  Description: Simple Stacker game for a Charlieplexed circuit of LEDs.
// ************************************************************************

int pins[] = {13, 12, 11, 10};
int vcc_perms[4][3][2] = { // Given {{x0, y0}, ...}, x0 and y0 pin range is 0 -> 3
  { {2, 0}, {3, 1}, {3, 0} },
  { {0, 2}, {1, 3}, {0, 3} },
  { {1, 0}, {2, 1}, {3, 2} },
  { {0, 1}, {1, 2}, {2, 3} }
};

int x_state = 1;
bool x_dir = true; // True = right

int state_time = 0;
int STATE_TIME_THRES = 100; // Char movement speed

int current_level = 1;
int win_timer = 0;
bool game_won = false;
bool prev_btn_state = false;

int const BUTTON_PIN = 9;
int const LED_DURATION = 1; // Milliseconds

void setup() {
  // Put your setup code here, to run once:
  pinMode(BUTTON_PIN, INPUT);
}

void loop() {
  // Put your main code here, to run repeatedly:

  // Button input check
  bool button_state = !digitalRead(BUTTON_PIN);
  if (!button_state) {
    // Button not pressed
    state_time++;
    prev_btn_state = false;
  } else {
    // Check for the instance where the button is down
    if (!prev_btn_state) {
      if (x_state == 1 && current_level < 3) {
        current_level++;
      } else if (x_state == 1 && current_level == 3) {
        game_won = true;
      } else {
        resetGame();
      }
    }
    prev_btn_state = true;
  }

  // Change char position
  int *vcc_perm = vcc_perms[current_level][x_state]; // vcc_perms[row: current_level][column: x_state]
  int vcc_indice = vcc_perm[0];
  int gnd_indice = vcc_perm[1];
  lightLED(pins[vcc_indice], pins[gnd_indice], LED_DURATION);

  // Char movement delays
  if (!game_won) {
    if (state_time >= STATE_TIME_THRES) {
      state_time = 0;
      updateState();
    }
  } else {
    lightLED(pins[1], pins[2], LED_DURATION);
    win_timer++;
    if (win_timer >= 500) {
      resetGame();
    }
  }

  // Level completed dots
  updateLevel();

  // Reference dot
  lightLED(10, 12, LED_DURATION);
}

void resetGame() {
  // Reset the variables
  state_time = 0;
  x_state = 1;
  current_level = 1;
  x_dir = true;
  STATE_TIME_THRES = 100;
  game_won = false;
}

void updateLevel() {
  // Update the level difficulty
  if (current_level == 2) {
    STATE_TIME_THRES = 50;

    // End of level one
    lightLED(pins[1], pins[3], LED_DURATION);
  } else if (current_level == 3) {
    STATE_TIME_THRES = 30;

    // End of level one and two
    lightLED(pins[1], pins[3], LED_DURATION);
    lightLED(pins[2], pins[1], LED_DURATION);
  }
}

// Updates the char state using variable x_state. This variable
// decides where the char will end up next on the matrix.
void updateState() {
  // Change char direction at a wall
  if (x_state == 2 && x_dir || x_state == 0 && !x_dir) {
    x_dir = !x_dir;
  }

  // Increment char position in set direction
  if (x_dir) {
    x_state++;
  } else {
    x_state--;
  }
}

// Lights a single LED for a given duration. This function is intended
// to be looped indefinitely in order to switch on multiple LEDs.
//
// Inputs:  vcc_pin - integer for pin # of positive voltage
//          gnd_pin - integer for pin # of ground
//          duration - how long the LED to stay switched on
//
void lightLED(int vcc_pin, int gnd_pin, int duration) {
  // Turn LED on
  pinMode(gnd_pin, OUTPUT);
  pinMode(vcc_pin, OUTPUT);

  digitalWrite(gnd_pin, LOW);
  digitalWrite(vcc_pin, HIGH);

  // Leave on for this duration
  delay(duration);

  // Turn LED off and neutralise pins
  pinMode(gnd_pin, INPUT);
  pinMode(vcc_pin, INPUT);

  digitalWrite(vcc_pin, LOW);
}

