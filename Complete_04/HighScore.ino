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

  void incCharIndex() { 
    if (this->charIndex < 2) this->charIndex++;
  }

  void decCharIndex() {
    if (this->charIndex > 0) this->charIndex--;
  }

  void incChar() {
    
    if (this->chars[this->charIndex] == 'Z') {
      this->chars[this->charIndex] = 'A'; 
    } 
    else { 
      this->chars[this->charIndex]++;
    } 

  }

  void decChar() {
    
    if (this->chars[this->charIndex] == 'A') {
      this->chars[this->charIndex] = 'Z'; 
    } 
    else { 
      this->chars[this->charIndex]--;
    } 

  }

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


  // Test to see if hte new score deserves a place on the high score table, if so make room for it and return the index ..
  
  uint8_t saveScore(uint16_t score) {

    uint8_t newIndex = DO_NOT_EDIT_SLOT;


    // Look to see if the provided score is higher than any existing ones ..

    for (uint8_t x = 0; x < MAX_NUMBER_OF_SLOTS; x++) {

      Slot test = getSlot(x);

      if (test.score <= score) {

        newIndex = x;
        break;

      }

    }


    // If a slot was found then ripple all those below it down ..

    if (newIndex < DO_NOT_EDIT_SLOT) {

      for (uint8_t x = MAX_NUMBER_OF_SLOTS - 1; x > newIndex; x--) {

        Slot slot = getSlot(x - 1);
        slot.slotNumber = x;
        saveSlot(slot);

      }


      // .. and save the new score with the default AAA name ..

      Slot newSlot;
      newSlot.slotNumber = newIndex;
      newSlot.chars[0] = 'A';
      newSlot.chars[1] = 'A';
      newSlot.chars[2] = 'A';
      newSlot.score = score;
      saveSlot(newSlot);

    }

    return newIndex;


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

		case GameState::PlayGame_Init: 
      playGame_Init();
      [[fallthrough]]

		case GameState::PlayGame: 
      playGame();
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

	if (arduboy.justPressed(A_BUTTON)) {
    gameState = GameState::PlayGame_Init;
	}

	if (arduboy.justPressed(B_BUTTON)) {
    gameState = GameState::HighScore_Init;
	}

  // Render screen ..

  arduboy.setCursor(26, 20);
  arduboy.print(F("Title Screen!"));

  arduboy.setCursor(43, 40);
  arduboy.print(F("Press A"));

}


// ----------------------------------------------------------------------------
//  Game state ..
//
void playGame_Init() {

  gamePlayVars.reset();
  gameState = GameState::PlayGame;

}

void playGame() { 


	// Handle input ..

	if (arduboy.justPressed(UP_BUTTON)) {
    gamePlayVars.score++;
	}

	if (arduboy.justPressed(DOWN_BUTTON)) {
    gamePlayVars.score--;
	}

	if (arduboy.justPressed(A_BUTTON)) {
    gameState = GameState::HighScore_Init;
	}


  // Render screen ..

  arduboy.setCursor(32, 20);
  arduboy.print(F("Play Game!"));
  arduboy.setCursor(32, 40);
  arduboy.print(F("Score: "));
  arduboy.print(gamePlayVars.score);

}


// --------------------------------------------------------------------------------------
//  High Score state
//
void highScore_Init() {

  highScoreVars.slot.reset();
  highScoreVars.slot.score = gamePlayVars.score;
  highScoreVars.slot.slotNumber = eeprom_Utils.saveScore(gamePlayVars.score);

  gameState = GameState::HighScore;

}

void highScore() {

  uint8_t xOffset = 28;
  uint8_t yOffset = 3;

  for (uint8_t x = 0; x < MAX_NUMBER_OF_SLOTS; x++) {

    Slot slot = eeprom_Utils.getSlot(x);

    arduboy.setCursor(xOffset, yOffset);

    if (x != highScoreVars.slot.slotNumber) {


      // Render existing score details ..

      arduboy.print(static_cast<char>(slot.chars[0]));
      arduboy.print(static_cast<char>(slot.chars[1]));
      arduboy.print(static_cast<char>(slot.chars[2]));

    }
    else {

      // Render new deatils ..

      arduboy.print(static_cast<char>(highScoreVars.slot.chars[0]));
      arduboy.print(static_cast<char>(highScoreVars.slot.chars[1]));
      arduboy.print(static_cast<char>(highScoreVars.slot.chars[2]));

      // Render underline ..

      arduboy.drawFastHLine(xOffset + (highScoreVars.slot.charIndex * 6), yOffset - 2, 5);
      arduboy.drawFastHLine(xOffset + (highScoreVars.slot.charIndex * 6), yOffset + 8, 5);

    }


    arduboy.setCursor(xOffset + 40, yOffset);

    if (slot.score < 1000)  arduboy.print("0");
    if (slot.score < 100)   arduboy.print("0");
    if (slot.score < 10)    arduboy.print("0");

    arduboy.print(slot.score);

    yOffset = yOffset + 12;

  }


  // Handle buttons ..

  if (highScoreVars.slot.slotNumber != DO_NOT_EDIT_SLOT) {

    if (arduboy.justPressed(UP_BUTTON))       { highScoreVars.slot.incChar(); }
    if (arduboy.justPressed(DOWN_BUTTON))     { highScoreVars.slot.decChar(); }
    if (arduboy.justPressed(LEFT_BUTTON))     { highScoreVars.slot.decCharIndex(); } 
    if (arduboy.justPressed(RIGHT_BUTTON))    { highScoreVars.slot.incCharIndex(); } 

    if (arduboy.justPressed(A_BUTTON)) { 
      
      eeprom_Utils.saveSlot(highScoreVars.slot);
      highScoreVars.slot.reset(); 
      gamePlayVars.reset();
      
    }

  }
  else {

    if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) { 
    
      gameState = GameState::Title_Init; 
      
    }
    
  }

}

