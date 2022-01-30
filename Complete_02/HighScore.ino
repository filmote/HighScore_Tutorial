#include <Arduboy2.h>

// Defines ----------------------------------------------------------------------

#define DO_NOT_EDIT_SLOT              255
#define MAX_NUMBER_OF_SLOTS           5


// -------------------------------------------------------------------------------
// Slot structure.  Holds a single high score entry.

struct Slot {
  
  uint8_t slotNumber;
  uint8_t charIndex;
  uint8_t chars[3];
  uint16_t score;

  void reset() {

    this->slotNumber = DO_NOT_EDIT_SLOT;
    this->charIndex = 0;
    this->chars[0] = 'A';
    this->chars[1] = 'A';
    this->chars[2] = 'A';
    this->score = 0;

  }

};


// -------------------------------------------------------------------------------
// Utilities to read and write detqails to the EEPROM

struct EEPROM_Utils {

  Slot slots[5];    
    

  // Initiliase the EEPROM (well the array anyway) ..

  void initEEPROM() {

    for (uint8_t i = 0; i < MAX_NUMBER_OF_SLOTS; i++) {
      slots[i].chars[0] = 'A' + i;
      slots[i].chars[1] = 'A' + i;
      slots[i].chars[2] = 'A' + i;
      slots[i].score = 5 - i;
    }

  }


  // Get details of slot number x.
  
  Slot &getSlot(uint8_t x) {

    return slots[x];

  }


  // Save a slot ..

  void saveSlot(Slot &slot) {

      slots[slot.slotNumber].chars[0] = slot.chars[0];
      slots[slot.slotNumber].chars[1] = slot.chars[1];
      slots[slot.slotNumber].chars[2] = slot.chars[2];
      slots[slot.slotNumber].score = slot.score;

  }

};


// -------------------------------------------------------------------------------
//   Start of Game 
// -------------------------------------------------------------------------------

Arduboy2 arduboy;
EEPROM_Utils eeprom_Utils;

enum GameState {
  Title_Init,
  Title,
  PlayGame_Init,
  PlayGame,
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
  eeprom_Utils.initEEPROM();

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

    Slot slot = eeprom_Utils.getSlot(x);


    // Render existing score details ..

    arduboy.setCursor(xOffset, yOffset);

    arduboy.print(static_cast<char>(slot.chars[0]));
    arduboy.print(static_cast<char>(slot.chars[1]));
    arduboy.print(static_cast<char>(slot.chars[2]));

    arduboy.setCursor(xOffset + 40, yOffset);

    if (slot.score < 1000)  arduboy.print("0");
    if (slot.score < 100)   arduboy.print("0");
    if (slot.score < 10)    arduboy.print("0");

    arduboy.print(slot.score);

    yOffset = yOffset + 12;

  }


  // Handle buttons ..

  if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) { 
  
    gameState = GameState::Title_Init; 
    
  }

}

