# High Scores and Slots

In the previous exercise, we wrote a single high score entry to the EEPROM and retrieved it for display on our High Score screen.  What if we want to show the top five (or ten!) high scores in order?  Strap yourself in as we extend our knowledge to implement this.

In this section, we will put the EEPROM aside and manipulate our high score table in an array in memory to keep it simple.  In a later section, we will come back and add the EEPROM code.

# What are Slots?

Our high score table consists of multiple entries, that I have called a `Slot`.  Each slot consists of a three-character array to store the player's initials and the score achieved.  A `Slot` can be easily represented as a simple structure which, by now, you will have seen a number of times before.

```cpp
struct Slot {
  
  uint8_t chars[3];
  uint16_t score;

  void reset() {
    ...
  }

};
```

Our `Slot` structure will used to create an array (or high score table) using code similar to that shown below:

```cpp
Slot slots[5];
```

Being a structure, we can also incorporate functions within it to perform actions like resetting the variables. Later, we will add extra functions to scroll the initial characters up and down through the alphabet.

>Your Turn:<br/><br/>
>Complete the `reset()` function to set the three initials to the letter 'A' (in capitals) and the `score` to zero.  


# Creating and Initialising the High Score Table

Now that we have a structure to represent each entry on our high score table, we can create an array of them and a function to initialise the array with some meaningful data.  The code below will populate the first entry in the high score table with the initials 'AAA' and a score of 5, the second entry will be populated with 'BBB' and a score of 4 .. and so on.  This uses a simple trick to increase the letters - C++ sees the letter 'A' as a value (actually 65!) and adding 1 to it produces the letter 'B'. 

```cpp
Slot slots[MAX_NUMBER_OF_SLOTS];    

void initHighScoreTable() {

  for (uint8_t i = 0; i < MAX_NUMBER_OF_SLOTS; i++) {
    slots[i].chars[0] = 'A' + i;
    slots[i].chars[1] = 'A' + i;
    slots[i].chars[2] = 'A' + i;
    slots[i].score = 5 - i;
  }

}
```

>Your Turn:<br/><br/>
>Copy and paste the initialisation code into the `initHighScoreTable` function.  If you like, you can change the code to initialise the table with other letters and scores.


# Retrieving Slots / Rendering the High Score Table

The `getSlot()` function, shown below, retrieves a single slot from the array using the variable `x` as an index.  

```cpp
Slot getSlot(uint8_t x) {

  return slots[x];

}
```

With this function, you can retrieve the entire array, slot by slot, and render the high score table.

>Your Turn:<br/><br/>
>Alter the `highScore()` function to retrieve the slots in sequence and render the high score table.<br/><br/>
>As you did previously, use the `static_cast<char>(..)` technique to convert the integer values back to characters and zero-pad the score to produce a retro looking effect.

In the next section, we will add a function to save a new game score and allow the player to enter their initials.An