#include <Arduboy2.h>

// Defines ----------------------------------------------------------------------

#define MAX_NUMBER_OF_SLOTS           5


// -------------------------------------------------------------------------------
// Slot structure.  Holds a single high score entry.

struct Slot {
  
  uint8_t chars[3];
  uint16_t score;

  void reset() {
    
    // Your Turn: provide code to reset the slot structure.

  }

};


// -------------------------------------------------------------------------------
// Utilities to read and write details to the EEPROM

Slot slots[MAX_NUMBER_OF_SLOTS];    
  

// Initiliase the EEPROM (well the array anyway) ..

void initHighScoreTable() {

  // Your Turn: copy and paste the initialisation code from the instructions

}


// Get details of slot number x.

Slot getSlot(uint8_t x) {

  return slots[x];

}


// -------------------------------------------------------------------------------
//   Start of Game 
// -------------------------------------------------------------------------------

Arduboy2 arduboy;

enum GameState {
  Title_Init,
  Title,
  HighScore_Init,
  HighScore,
};

struct TitleScreenVars { };

struct GamePlayVars {

  uint8_t score = 0;

  void reset() {
    score = 0;
  }
  
};

struct HighScoreVars {

  Slot slot;

};

TitleScreenVars titleScreenVars;
GamePlayVars gamePlayVars;
HighScoreVars highScoreVars;

GameState gameState = GameState::Title_Init;


void setup() {

  arduboy.boot();
  initHighScoreTable();

  gameState = GameState::Title_Init; 

}

void loop() {

  if (!arduboy.nextFrame()) return;
  arduboy.pollButtons();
  arduboy.clear();

  switch (gameState) {

    case GameState::Title_Init:
      title_Init();
      [[fallthrough]]

    case GameState::Title:
      title();
      break;

    case GameState::HighScore_Init: 
      highScore_Init();
      [[fallthrough]]

    case GameState::HighScore: 
      highScore();
      break;

    default: break;  

  }

  arduboy.display();

}


// ----------------------------------------------------------------------------
//  Title state ..
//
void title_Init() {

  gameState = GameState::Title;

}

void title() {

  // Handle input ..

  if (arduboy.justPressed(B_BUTTON)) {
    gameState = GameState::HighScore_Init;
  }

  // Render screen ..

  arduboy.setCursor(26, 20);
  arduboy.print(F("Title Screen!"));

  arduboy.setCursor(43, 40);
  arduboy.print(F("Press B"));

}


// --------------------------------------------------------------------------------------
//  High Score state
//
void highScore_Init() {

  gameState = GameState::HighScore;

}

void highScore() {

  uint8_t xOffset = 28;
  uint8_t yOffset = 3;

  for (uint8_t x = 0; x < MAX_NUMBER_OF_SLOTS; x++) {

    Slot slot = getSlot(x);
    
    // Your Turn: retrieve and render all slots

    yOffset = yOffset + 12;

  }


  // Handle buttons ..

  if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) { 
  
    gameState = GameState::Title_Init; 
    
  }

}

