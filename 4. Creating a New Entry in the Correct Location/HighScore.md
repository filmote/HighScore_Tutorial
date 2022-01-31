# Inserting the New Score in the Correct Place!

Up until now, we have been randomly choosing a location in the high score table to store the new score.  In this (short) section, we will look at how to detect the correct position to store the new score and move the existing entries down to make space.

>Your Turn:<br/><br/>
>In the `saveScore()` function, retrieve each slot and compare its score with the score passed in as a parameter.  If the retrieved score is lower than the passed parameter, store the current index for later.  In the code, you will see that there is already a variable named `newIndex` which you can use to store the index.  It is initialised with the default `DO_NOT_EDIT_SLOT` indicating that no place was found in the table for the new score.

If a location is found in the high score table for the new entry, we must move any existing records down to make room.  

The following code snippet test that a result was found and, if so, loops from the last slot entry in the table and replaces it with the second last one and so forth until it reaches the slot indicated by the `newIndex` variable.  Slots are retrieved and their index changed before saving them again - in effect moving them.

```cpp
uint8_t saveScore(uint16_t score) {

  uint8_t newIndex = DO_NOT_EDIT_SLOT;

  ...

  if (newIndex < DO_NOT_EDIT_SLOT) {

    for (uint8_t x = MAX_NUMBER_OF_SLOTS - 1; x > newIndex; x--) {

      Slot slot = getSlot(x - 1);
      slot.index = x;
      saveSlot(slot);

    }


    // .. and save the new score with the default AAA name ..

    Slot newSlot;
    ...

  }

  return newIndex;

}
```

>Your Turn:<br/><br/>
>In the `saveScore()` function, insert code that creates and saves a new entry in the correct location.  As usual, make the initials of the slot equal to 'AAA' and the score equal to the parameter passed.

At this stage, our high scores are stored properly in the correct place in the table.  However, they are still all stored in memory and therefore would not survive the power being turned off and back on again.  In the next section, we will swap the array out for proper EEPROM code!
