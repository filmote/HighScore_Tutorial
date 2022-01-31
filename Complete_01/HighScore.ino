#include <Arduboy2.h>


// Defines ----------------------------------------------------------------------

#define EEPROM_START                  EEPROM_STORAGE_SPACE_START + 300


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

GameState gameState = GameState::Title_Init;

void setup() {

  arduboy.boot();
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

  uint16_t score = 1234;

  // Your Turn: Save data to the EEPROM

  gameState = GameState::HighScore;

}

void highScore() {

  arduboy.setCursor(28, 20);

  arduboy.print(static_cast<char>(EEPROM.read(EEPROM_START)));
  
  // Your Turn: Retrieve and print initials 2 & 3 ..

  arduboy.print(static_cast<char>(EEPROM.read(EEPROM_START + 1)));
  arduboy.print(static_cast<char>(EEPROM.read(EEPROM_START + 2)));

  arduboy.setCursor(68, 20);

  // Your Turn: Retrieve and print the score ..


  // Handle input ..

  if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) {
    gameState = GameState::Title_Init;
  }


}

