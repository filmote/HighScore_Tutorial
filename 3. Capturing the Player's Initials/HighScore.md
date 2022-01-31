# Saving a New Score and Initials

In this section we will add the ability for a player to save a new high score and their initials in the High Score table.

Before proceeding, you will notice that an additional `playGame` state has been added to the code since the last section.  It will emulate a player finishing a game and will allow them to enter a score to then test the high score functionality.  As you can see by reviewing the code, pressing the Up and Down buttons will alter the `gamePlayVars.score` value and pressing the A button will progress you to the High Score screen.

Hopefully, the various screen states are second nature to you now.  If not, I encourage you to review the 'State Machine' tutorial.


# Altering the Slot Structure

As previously hinted at, `structs` allow related variables and functions that operate over them to be grouped together.  We have previously added a `reset()` function to our `Slot` structure and in this section we will add extra functions to support the entry of the initials.

The initials are stored in a 3 character array, called `chars`.  When entering the initials, the player will scroll up and down through the alphabet to find the letter they want before moving left or right to the previous / next initial.  

To track which initial is currently being edited, we will add an extra variable called `charIndex`.  When the player clicks the Left or Right buttons to swap which initial they are editing, we will increase or decrease the index accordingly.

The code below shows the inclusion of the `charIndex` variable and a new function `incCharIndex()` that will be executed when the player clicks the Right button.  As you can see it will increase the index by one until it reaches the index of our third initial (index value two).


```cpp
struct Slot {
  
  uint8_t index;
  uint8_t charIndex;
  uint8_t chars[3];
  uint16_t score;

  void incCharIndex() { 
    if (this->charIndex < 2) this->charIndex++;
  }

  ...
};
```

>Your Turn:<br/><br/>
>Add a `decCharIndex()` function to decrease the `charIndex` variable.  Be sure to add validation that prevents the index from moving outside of our range of initials (0 - 2).

Now we can create a function that cycles the selected character up and down through the alphabet.  Note how it updates the selected character in the `chars` array (using the `charIndex` as an index) and increases the value until it reaches 'Z' at which point it will wrap around back to 'A'. 

```cpp
struct Slot {
  ...

  void incChar() {
    
    if (this->chars[this->charIndex] == 'Z') {
      this->chars[this->charIndex] = 'A'; 
    } 
    else { 
      this->chars[this->charIndex]++;
    } 

  }

  ...

};
```

>Your Turn:<br/><br/>
>Add a `decChar()` function that will decrease the selected character.  Make sure your code scrolls backwards through the alphabet and when it reaches 'A' it should wrap around to 'Z'.


# Inserting a New Slot

When a player achieves a new high score, the program must determine what position in the high score table the new entry should be inserted.  In a later section, we will look at how to detect this and shuffle the existing array entries to make room for the new entry but, for the moment, we will just keep it simple and randomly pick a location to insert the new score and then initialise the `chars` array to the default 'AAA'.

The `saveScore()` function detects whether a non-zero score has been passed and, if so, calculates a random slot to overwrite.  The index of the overwritten slot is returned to the calling program to allow for the later entry of the initials.  If a zero score is passed, no slot is overwritten and a default value of `DO_NOT_EDIT_SLOT` is returned as the index.  Later when we see this value we will know that the user's score wasn't good enough to make the table.


```cpp
uint8_t saveScore(uint16_t score) {

  uint8_t newIndex = DO_NOT_EDIT_SLOT;

  if (score > 0) {

    newIndex = random(0, MAX_NUMBER_OF_SLOTS);
    ...

  }

  return newIndex;

}
```

>Your Turn:<br/><br/>
>Update the `saveScore()` function to save the score at the randomly selected location and to reset the `chars` array to the default 'AAA'.  You can do this yourself or you can use the `reset()` function which is part of the `Slot` structure.


# Finalising the Slot Details

After the player has completed the entry of their initials, we must write these back to the slot that was allocated via the `saveScore()` function above. The score, initials and even the index where to store the entry are all passed in a single `Slot` structure.

```cpp
void saveSlot(Slot &slotToSave) {

    slots[slotToSave.index].chars[0] = slotToSave.chars[0];
    slots[slotToSave.index].chars[1] = slotToSave.chars[1];
    slots[slotToSave.index].chars[2] = slotToSave.chars[2];
    slots[slotToSave.index].score = slotToSave.score;

}
```


# Detecting a New High Score

As mentioned earlier, a new `playGame` state has been added to the code to allow the player to enter a high score and watch how the entry is added to the high score table.

The variables used in the `highScore` state are defined in a structure called `highScoreVars`, as shown below.  Within this, a `Slot` variable has been added to capture the score and initials entered by the player before being written to the high score table (and ultimately the EEPROM!).

Before displaying the `highScore` state, we need to determine whether the score entered in the `playGame` state is high enough to be added to the table.  We can do this in the `highScore_init()` function by resetting the structure and then populating the score before calling our existing `saveScore()` function.  The score entered by the player in the `gamePlay` state is stored in a variable named `gamePlayVars.score` and it is still accessible within the `highScore_init()` function.


```cpp
struct HighScoreVars {
  Slot slot;
};

void highScore_Init() {

  highScoreVars.slot.reset();
  highScoreVars.slot.score = gamePlayVars.score;
  highScoreVars.slot.index = saveScore(gamePlayVars.score);

  gameState = GameState::HighScore;

}
```
 
Note that the index returned from the `saveScore()` function is saved into the `highScoreVars.slot` variable so that we can refer to it later.  If you recall, a score that is too low for the table will return a default value of `DO_NOT_EDIT_SLOT` to indicate this.  This value happens to equal `255` however it is an arbitrary value and could be changed as long as it is not a valid table index, ie. values 0 - 4.


# Rendering the High Score Table

In the previous section, we rendered each high score entry from data retrieved from the array of slots.  We need to change the existing code to render the slots differently depending on whether they are being edited or not.

As we iterate through the slots, we can determine if the current one is being edited simply by comparing its position to the index returned from the `saveScore()` function.  This value was saved in the index of the `highScoreVars.slot` in the `highScore_Init()` function.  If the two value match then this is the slot being edited and the initials to be rendered should come from the `highScoreVars.slot` variable.  

The score requires no special handling as it has already been saved to the slot and will equal that in the `highScoreVars.slot` as we initialised it within the `highScore_init()` function to be so.

```cpp
void highScore() {

  uint8_t xOffset = 28;
  uint8_t yOffset = 3;

  for (uint8_t x = 0; x < MAX_NUMBER_OF_SLOTS; x++) {

    Slot slot = getSlot(x);

    arduboy.setCursor(xOffset, yOffset);

    if (x != highScoreVars.slot.index) {

      arduboy.print(static_cast<char>(slot.chars[0]));
      arduboy.print(static_cast<char>(slot.chars[1]));
      arduboy.print(static_cast<char>(slot.chars[2]));

    }
    else {

      // Render the details from the slot being modified.

      arduboy.drawFastHLine(xOffset + (highScoreVars.slot.charIndex * 6), yOffset - 2, 5);
      arduboy.drawFastHLine(xOffset + (highScoreVars.slot.charIndex * 6), yOffset + 8, 5);

    }

    ...
    arduboy.print(slot.score);

    yOffset = yOffset + 12;

  }

  ...

}
```

>Your Turn:<br/><br/> 
> Add code to render the initials from the `highScoreVars.slot` variable for the record being edited.

# Capture the Player's Initials

OK, we are almost there ..

We can now update the player's initials in the `Slot` structure in response to them pressing the Up, Down left and Right buttons.  Once they have completed the entry, pressing the A button will save the new details to the high score table and reset the `highscore.slot` variable.  The `reset()` function sets the slot's `index` to the value of `DO_NOT_EDIT_SLOT` thus effectively ending the editing mode.  From that point onwards, pressing the A or B button will return the player to the title screen.

```cpp
void highScore() {
  
  ...

  if (highScoreVars.slot.index != DO_NOT_EDIT_SLOT) {

    uint8_t charIndex = highScoreVars.slot.charIndex;

    if (arduboy.justPressed(UP_BUTTON))       { highScoreVars.slot.incChar(); }
    ...

    if (arduboy.justPressed(A_BUTTON)) { 
      
      saveSlot(highScoreVars.slot);
      highScoreVars.slot.reset(); 
      gamePlayVars.reset();
      
    }

  }
  else {

    if (arduboy.justPressed(A_BUTTON) || arduboy.justPressed(B_BUTTON)) { 
    
      gameState = GameState::Title_Init; 
      
    }
    
  }

  ...
};
```

>Your Turn:<br/><br/> 
>The code shows how the current initial being editted is increased though the alphabet when the player presses the Up button.  Add similar logic to decrease the initial when the Down button is pressed.<br/><br/>
>Add code immediately below this to change the initial being modified using the Left and Right buttons.  This should call the `decCharIndex()` and `incCharIndex()` functions respectively.

Wow, we have a high score table working!

In the next section, we will update the logic in `saveScore()` to find the right index in the high score table and move the existing slots to make room.