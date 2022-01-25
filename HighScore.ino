#include <Arduboy2.h>


// Defines ----------------------------------------------------------------------

#define EEPROM_GUARD_LETTER_1         'X'
#define EEPROM_GUARD_LETTER_2         'Y'
#define EEPROM_START                  EEPROM_STORAGE_SPACE_START + 300
#define EEPROM_START_C1               EEPROM_START
#define EEPROM_START_C2               EEPROM_START + 1
#define EEPROM_TOP_START              EEPROM_START + 2
#define EEPROM_ENTRY_SIZE             5
#define DO_NOT_EDIT_SLOT              255
#define MAX_NUMBER_OF_SLOTS           5


// -------------------------------------------------------------------------------
// Slot structure.  Holds a single high score entry

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
    
    
 /* -----------------------------------------------------------------------------
  *   Is the EEPROM initialised?
  *
  *   Looks for the characters 'X' and 'Y' in the first two bytes of the EEPROM
  *   memory range starting from byte EEPROM_START.  If not found, it resets the
  *   settings back to default ..
  */
  void initEEPROM() {

    uint8_t c1 = EEPROM.read(EEPROM_START_C1);
    uint8_t c2 = EEPROM.read(EEPROM_START_C2);

    if (c1 != EEPROM_GUARD_LETTER_1 || c2 != EEPROM_GUARD_LETTER_2) {

      uint16_t score = 0;
      EEPROM.update(EEPROM_START_C1, EEPROM_GUARD_LETTER_1);
      EEPROM.update(EEPROM_START_C2, EEPROM_GUARD_LETTER_2);

      for (uint8_t x = 0; x < MAX_NUMBER_OF_SLOTS; x++) {

        EEPROM.update(EEPROM_TOP_START + (EEPROM_ENTRY_SIZE * x),     'A' + x);
        EEPROM.update(EEPROM_TOP_START + (EEPROM_ENTRY_SIZE * x) + 1, 'A' + x);
        EEPROM.update(EEPROM_TOP_START + (EEPROM_ENTRY_SIZE * x) + 2, 'A' + x);

        score = MAX_NUMBER_OF_SLOTS - x;
        EEPROM.put(EEPROM_TOP_START + (EEPROM_ENTRY_SIZE * x) + 3, score);

      }

    }

  }


 /* -----------------------------------------------------------------------------
  *   Get details of slot number x.
  */
  void getSlot(uint8_t x, Slot &slot) {

    slot.chars[0] = EEPROM.read(EEPROM_TOP_START + (EEPROM_ENTRY_SIZE * x));
    slot.chars[1] = EEPROM.read(EEPROM_TOP_START + (EEPROM_ENTRY_SIZE * x) + 1);
    slot.chars[2] = EEPROM.read(EEPROM_TOP_START + (EEPROM_ENTRY_SIZE * x) + 2);

    uint16_t score = 0;
    EEPROM.get(EEPROM_TOP_START + (EEPROM_ENTRY_SIZE * x) + 3, score);
    slot.score = score;

  }


 /* -----------------------------------------------------------------------------
  *   Save score and return index.  if the reserved value DO_NOT_EDIT_SLOT (255)
  *   is returned, it means the score provided was not good enough!
  */
  uint8_t saveScore(uint16_t score) {

    uint8_t newIndex = DO_NOT_EDIT_SLOT;


    // Look to see if the provided score is higher than any existing ones ..

    for (uint8_t x = 0; x < MAX_NUMBER_OF_SLOTS; x++) {

      uint16_t slotScore = 0;
      EEPROM.get(EEPROM_TOP_START + (EEPROM_ENTRY_SIZE * x) + 3, slotScore);

      if (slotScore <= score) {

        newIndex = x;
        break;

      }

    }


    // If a slot was found then ripple all those below it down ..

    if (newIndex < DO_NOT_EDIT_SLOT) {

      for (uint8_t x = MAX_NUMBER_OF_SLOTS - 1; x > newIndex; x--) {

        Slot slot;
        getSlot(x - 1, slot);

        EEPROM.update(EEPROM_TOP_START + (EEPROM_ENTRY_SIZE * x), slot.chars[0]);
        EEPROM.update(EEPROM_TOP_START + (EEPROM_ENTRY_SIZE * x) + 1, slot.chars[1]);
        EEPROM.update(EEPROM_TOP_START + (EEPROM_ENTRY_SIZE * x) + 2, slot.chars[2]);
        EEPROM.put(EEPROM_TOP_START + (EEPROM_ENTRY_SIZE * x) + 3, slot.score);

      }


      // .. and save the new score with the default AAA name ..

      EEPROM.update(EEPROM_TOP_START + (EEPROM_ENTRY_SIZE * newIndex), 'A');
      EEPROM.update(EEPROM_TOP_START + (EEPROM_ENTRY_SIZE * newIndex) + 1, 'A');
      EEPROM.update(EEPROM_TOP_START + (EEPROM_ENTRY_SIZE * newIndex) + 2, 'A');
      EEPROM.put(EEPROM_TOP_START + (EEPROM_ENTRY_SIZE * newIndex) + 3, score);
      

    }

    return newIndex;

  }


 /* -----------------------------------------------------------------------------
  *   Save score characters only.  The score itself was saved when we created the 
  *   located the spot in the first place.
  */
  void writeChars(Slot &highscore) {

      EEPROM.update(EEPROM_TOP_START + (EEPROM_ENTRY_SIZE * highscore.slotNumber), highscore.chars[0]);
      EEPROM.update(EEPROM_TOP_START + (EEPROM_ENTRY_SIZE * highscore.slotNumber) + 1, highscore.chars[1]);
      EEPROM.update(EEPROM_TOP_START + (EEPROM_ENTRY_SIZE * highscore.slotNumber) + 2, highscore.chars[2]);

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
  highScoreVars.slot.slotNumber = eeprom_Utils.saveScore(gamePlayVars.score);

  gameState = GameState::HighScore;

}

void highScore() {

  uint8_t xOffset = 28;
  uint8_t yOffset = 3;

  for (uint8_t x = 0; x < MAX_NUMBER_OF_SLOTS; x++) {

    Slot slot;
    eeprom_Utils.getSlot(x, slot);

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

    uint8_t charIndex = highScoreVars.slot.charIndex;

    if (arduboy.justPressed(UP_BUTTON))       { highScoreVars.slot.incChar(); }
    if (arduboy.justPressed(DOWN_BUTTON))     { highScoreVars.slot.decChar(); }
    if (arduboy.justPressed(LEFT_BUTTON))     { highScoreVars.slot.decCharIndex(); } 
    if (arduboy.justPressed(RIGHT_BUTTON))    { highScoreVars.slot.incCharIndex(); } 

    if (arduboy.justPressed(A_BUTTON)) { 
      
      eeprom_Utils.writeChars(highScoreVars.slot);
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
