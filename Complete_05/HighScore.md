# Back to the EEPROM

OK .. let's finish this off!

In this section, we will swap out all the code that refers to the in-memory array of slots and replace it with code that uses the EEPROM instead.

# More Constants, Anyone?

If you recall from the very first segment, when we read and write to the EEPROM we need to tell it which memory location to use.  In that first lesson we used a constant named `EEPROM_START` that pointed to a 'random' location 300 bytes into the EEPROM memory however we will need a few more constants to complete the code.  These are shown below and their use will become apparent as we progress.

```cpp
#define EEPROM_GUARD_LETTER_1         'X'
#define EEPROM_GUARD_LETTER_2         'Y'
#define EEPROM_ENTRY_SIZE             5

#define EEPROM_START                  EEPROM_STORAGE_SPACE_START + 300
#define EEPROM_START_C1               EEPROM_START
#define EEPROM_START_C2               EEPROM_START + 1
#define EEPROM_SCORES_START           EEPROM_START + 2
```


# Initialising the High Score Table in EEPROM

So why did we start saving 300 bytes into the EEPROM memory?

Most games on the Arduboy use 10 - 20 bytes of EEPROM at most to save their high scores and other information.  If all games saved their scores at the start of the EEPROM memory, it would make it impossible to swap from one game to another while keeping the first game's scores intact.  The second game's high scores would immediately overwrite the first.  By randomly picking a memory location somewhere in the middle of the EEPROM, we reduce the likelihood of memory collisions.  However, nothing is guaranteed as some other game may also use the location you randomly chose!

But this brings us to a new problem .. what if there is data stored from some other game already in the memory locations we want to use?  In the case of a high score, it would simply show garbage in the high score table but in the case where a game's progress is stored, it may lead to unpredictable results.

To avoid this, we need to make sure our memory is initialised before we start using it.  One way is to write something specific in a known range of cells and use them to test to see if another program has overwritten and corrupted the memory.  This is not fool-proof either as something may have written over the tail-end of our data but not over the 'guard' values however it a good start!

This approach is shown below.  Two known memory locations, `EEPROM_START_C1` and `EEPROM_START_C2`, are retrieved and compared to two static values, `EEPROM_GUARD_LETTER_1` and `EEPROM_GUARD_LETTER_2`, to make sure the memory has been initialised.  If neither memory location's value matches the expected values, it is assumed the memory needs initialisation.

```cpp
void initHighScoreTable() {

  uint8_t c1 = EEPROM.read(EEPROM_START_C1);
  uint8_t c2 = EEPROM.read(EEPROM_START_C2);

  if (c1 != EEPROM_GUARD_LETTER_1 || c2 != EEPROM_GUARD_LETTER_2) {

    uint16_t score = 0;
    EEPROM.update(EEPROM_START_C1, EEPROM_GUARD_LETTER_1);
    EEPROM.update(EEPROM_START_C2, EEPROM_GUARD_LETTER_2);

    for (uint8_t x = 0; x < MAX_NUMBER_OF_SLOTS; x++) {

      EEPROM.update(EEPROM_SCORES_START + (EEPROM_ENTRY_SIZE * x),     'A' + x);
      // Your Turn: Complete the initialisation of each slot.

    }

  }

}
```

As part of the initialisation, the two 'guard' characters are written to the EEPROM and then the slots themselves are initialised.  The 'guard' bytes are stored in the first two locations of our memory range and the data for the slots start at the location indicated by the constant `EEPROM_SCORES_START`.  Each slot is 5 bytes long - three initials followed by a two-byte score - and a new constant, `EEPROM_ENTRY_SIZE`, is used to help calculate the memory locations to use.

> Your Turn:<br/><br/>
> Using the knowledge you gained in the first section of this tutorial, initialise the slots so that the first entry has initials of 'AAA' and a score of 5.  The second slot should have initials of 'BBB' and a score of 4 and so on.


# Retrieving a Slot

Using the same concepts to calculate the EEPROM memory locations as above, retrieving a single slot is child's play.

```cpp
Slot getSlot(uint8_t x) {

  Slot slot;

  slot.index = x;
  // Populate the slot variable with the retrieved EEPROM values

  return slot;

}
```

> Your Turn:<br/><br/>
> Using the knowledge you gained in the first section of this tutorial, retrieve a single slot's details from EEPROM.

# Writing a Slot

Writing the values of a single slot to EEPROM is equally as simple.  If you recall, the slot has an `index` variable which can be used to calculate the EEPROM memory locations. In the sample code below, the first initial has been done for you!

```cpp
void saveSlot(Slot &slotToSave) {

    EEPROM.update(EEPROM_SCORES_START + (EEPROM_ENTRY_SIZE * slotToSave.index), slotToSave.chars[0]);
    ...
 
}
```

> Your Turn:<br/><br/>
> Complete the remainder of the updates to save a complete slot.

And that's it!  From now on, every game you write should have a High Score feature!
